/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2025 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "directions.H"
#include "polyMesh.H"
#include "twoDPointCorrector.H"
#include "directionInfo.H"
#include "FaceCellWave.H"
#include "OFstream.H"
#include "meshTools.H"
#include "hexMatcher.H"
#include "Switch.H"
#include "globalMeshData.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

const Foam::NamedEnum<Foam::directions::directionType, 3>
Foam::directions::directionTypeNames_
{
    "e1",
    "e2",
    "e3"
};


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::directions::writeOBJ(Ostream& os, const point& pt)
{
    os << "v " << pt.x() << ' ' << pt.y() << ' ' << pt.z() << endl;
}


void Foam::directions::writeOBJ
(
    Ostream& os,
    const point& pt0,
    const point& pt1,
    label& vertI
)
{
    writeOBJ(os, pt0);
    writeOBJ(os, pt1);

    os << "l " << vertI + 1 << ' ' << vertI + 2 << endl;

    vertI += 2;
}


void Foam::directions::writeOBJ
(
    const fileName& fName,
    const primitiveMesh& mesh,
    const vectorField& dirs
)
{
    Pout<< "Writing cell info to " << fName << " as vectors at the cellCentres"
        << endl << endl;

    OFstream xDirStream(fName);

    label vertI = 0;

    forAll(dirs, celli)
    {
        const point& ctr = mesh.cellCentres()[celli];

        // Calculate local length scale
        scalar minDist = great;

        const labelList& nbrs = mesh.cellCells()[celli];

        forAll(nbrs, nbrI)
        {
            minDist = min(minDist, mag(mesh.cellCentres()[nbrs[nbrI]] - ctr));
        }

        scalar scale = 0.5*minDist;

        writeOBJ(xDirStream, ctr, ctr + scale*dirs[celli], vertI);
    }
}


void Foam::directions::check2D
(
    const twoDPointCorrector* correct2DPtr,
    const vector& vec
)
{
    if (correct2DPtr)
    {
        if (mag(correct2DPtr->planeNormal() & vec) > 1e-6)
        {
            FatalErrorInFunction
                << "is not normal to plane defined in dynamicMeshDict."
                << endl
                << "Either make case 3D or adjust vector."
                << exit(FatalError);
        }
    }
}


Foam::vectorField Foam::directions::propagateDirection
(
    const polyMesh& mesh,
    const bool useTopo,
    const polyPatch& pp,
    const vectorField& ppField,
    const vector& defaultDir
)
{
    // Seed all faces on patch
    labelList changedFaces(pp.size());
    List<directionInfo> changedFacesInfo(pp.size());

    if (useTopo)
    {
        forAll(pp, patchFacei)
        {
            label meshFacei = pp.start() + patchFacei;

            label celli = mesh.faceOwner()[meshFacei];

            if (!hexMatcher().isA(mesh, celli))
            {
                FatalErrorInFunction
                    << "useHexTopology specified but cell " << celli
                    << " on face " << patchFacei << " of patch " << pp.name()
                    << " is not a hex" << exit(FatalError);
            }

            const vector& cutDir = ppField[patchFacei];

            // Get edge(bundle) on cell most in direction of cutdir
            label edgeI = meshTools::cutDirToEdge(mesh, celli, cutDir);

            // Convert edge into index on face
            label faceIndex =
                directionInfo::edgeToFaceIndex
                (
                    mesh,
                    celli,
                    meshFacei,
                    edgeI
                );

            // Set initial face and direction
            changedFaces[patchFacei] = meshFacei;
            changedFacesInfo[patchFacei] =
                directionInfo
                (
                    faceIndex,
                    cutDir
                );
        }
    }
    else
    {
        forAll(pp, patchFacei)
        {
            changedFaces[patchFacei] = pp.start() + patchFacei;
            changedFacesInfo[patchFacei] =
                directionInfo
                (
                    -2,         // Geometric information only
                    ppField[patchFacei]
                );
        }
    }

    List<directionInfo> faceInfo(mesh.nFaces()), cellInfo(mesh.nCells());
    FaceCellWave<directionInfo> directionCalc
    (
        mesh,
        changedFaces,
        changedFacesInfo,
        faceInfo,
        cellInfo,
        mesh.globalData().nTotalCells() + 1
    );

    vectorField dirField(cellInfo.size());

    label nUnset = 0;
    label nGeom = 0;
    label nTopo = 0;

    forAll(cellInfo, celli)
    {
        label index = cellInfo[celli].index();

        if (index == -3)
        {
            // Never visited
            WarningInFunction
                << "Cell " << celli << " never visited to determine "
                << "local coordinate system" << endl
                << "Using direction " << defaultDir << " instead" << endl;

            dirField[celli] = defaultDir;

            nUnset++;
        }
        else if (index == -2)
        {
            // Geometric direction
            dirField[celli] = cellInfo[celli].n();

            nGeom++;
        }
        else if (index == -1)
        {
            FatalErrorInFunction
                << "Illegal index " << index << endl
                << "Value is only allowed on faces" << abort(FatalError);
        }
        else
        {
            // Topological edge cut. Convert into average cut direction.
            dirField[celli] = meshTools::edgeToCutDir(mesh, celli, index);

            nTopo++;
        }
    }

    reduce(nGeom, sumOp<label>());
    reduce(nTopo, sumOp<label>());
    reduce(nUnset, sumOp<label>());

    Info<< "Calculated local coords for " << defaultDir
        << endl
        << "    Geometric cut cells   : " << nGeom << endl
        << "    Topological cut cells : " << nTopo << endl
        << "    Unset cells           : " << nUnset << endl
        << endl;

    return dirField;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::directions::directions
(
    const polyMesh& mesh,
    const dictionary& dict,
    const twoDPointCorrector* correct2DPtr
)
{
    word coordSystem;
    wordList wantedDirs;

    if (dict.found("type"))
    {
        coordSystem = dict.lookup<word>("type");
        wantedDirs = dict.lookup<wordList>("directions");
    }
    else
    {
        coordSystem = dict.lookup<word>("coordinateSystem");
        wantedDirs = dict.lookup<wordList>("directions");
    }

    setSize(wantedDirs.size());

    bool wantE3 = false;
    bool wantE1 = false;
    bool wantE2 = false;
    label nDirs = 0;

    if (coordSystem != "fieldBased")
    {
        forAll(wantedDirs, i)
        {
            directionType wantedDir = directionTypeNames_[wantedDirs[i]];

            if (wantedDir == e3)
            {
                wantE3 = true;
            }
            else if (wantedDir == e1)
            {
                wantE1 = true;
            }
            else if (wantedDir == e2)
            {
                wantE2 = true;
            }
        }
    }


    if (coordSystem == "global")
    {
        const dictionary& globalDict = dict.optionalSubDict("globalCoeffs");

        vector e1(globalDict.lookup("e1"));
        check2D(correct2DPtr, e1);

        vector e2(globalDict.lookup("e2"));
        check2D(correct2DPtr, e2);

        vector e3 = e1 ^ e2;
        e3 /= mag(e3);

        Info<< "Global Coordinate system:" << endl
            << "     e1 : " << e1 << nl
            << "     e2 : " << e2 << nl
            << "     e3 : " << e3 << nl
            << endl;

        if (wantE3)
        {
            operator[](nDirs++) = vectorField(1, e3);
        }
        if (wantE1)
        {
            operator[](nDirs++) = vectorField(1, e1);
        }
        if (wantE2)
        {
            operator[](nDirs++) = vectorField(1, e2);
        }
    }
    else if (coordSystem == "patchLocal")
    {
        const dictionary& patchDict = dict.optionalSubDict("patchLocalCoeffs");

        const word patchName(patchDict.lookup("patch"));

        const label patchi = mesh.boundaryMesh().findIndex(patchName);

        if (patchi == -1)
        {
            FatalErrorInFunction
                << "Cannot find patch "
                << patchName
                << exit(FatalError);
        }

        // Take zeroth face on patch
        const polyPatch& pp = mesh.boundaryMesh()[patchi];

        vector e1(patchDict.lookup("e1"));

        const vector& n0 = pp.faceNormals()[0];

        if (correct2DPtr)
        {
            e1 = correct2DPtr->planeNormal() ^ n0;

            WarningInFunction
                << "Discarding user specified e1 since 2D case." << endl
                << "Recalculated e1 from face normal and planeNormal as "
                << e1 << endl << endl;
        }

        Switch useTopo(dict.lookupOrDefault("useHexTopology", false));

        vectorField e3Dirs;
        vectorField e1Dirs;

        if (wantE3 || wantE2)
        {
            e3Dirs =
                propagateDirection
                (
                    mesh,
                    useTopo,
                    pp,
                    pp.faceNormals(),
                    n0
                );

            if (wantE3)
            {
                this->operator[](nDirs++) = e3Dirs;
            }
        }

        if (wantE1 || wantE2)
        {
            e1Dirs =
                propagateDirection
                (
                    mesh,
                    useTopo,
                    pp,
                    vectorField(pp.size(), e1),
                    e1
                );


            if (wantE1)
            {
                this->operator[](nDirs++) = e1Dirs;
            }
        }
        if (wantE2)
        {
            tmp<vectorField> e2Dirs = e3Dirs ^ e1Dirs;

            this->operator[](nDirs++) = e2Dirs;
        }
    }
    else if (coordSystem == "fieldBased")
    {
        forAll(wantedDirs, i)
        {
            operator[](nDirs++) =
                vectorIOField
                (
                    IOobject
                    (
                        mesh.instance()/wantedDirs[i],
                        mesh,
                        IOobject::MUST_READ,
                        IOobject::NO_WRITE
                    )
                );
        }
    }
    else
    {
        FatalErrorInFunction
            << "Unknown coordinate system "
            << coordSystem << endl
            << "Known types are global, patchLocal and fieldBased"
            << exit(FatalError);
    }
}


// ************************************************************************* //
