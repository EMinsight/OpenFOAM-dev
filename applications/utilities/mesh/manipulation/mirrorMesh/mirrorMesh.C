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

Application
    mirrorMesh

Description
    Mirrors a mesh around a given plane.

\*---------------------------------------------------------------------------*/

#include "argList.H"
#include "Time.H"
#include "mirrorFvMesh.H"
#include "polyTopoChangeMap.H"
#include "hexRef8Data.H"
#include "systemDict.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "addNoOverwriteOption.H"
    #include "addMeshOption.H"
    #include "addRegionOption.H"
    #include "addDictOption.H"

    #include "setRootCase.H"
    #include "setMeshPath.H"
    #include "createTime.H"

    const word regionName =
        args.optionLookupOrDefault("region", polyMesh::defaultRegion);

    #include "setNoOverwrite.H"

    mirrorFvMesh mesh
    (
        IOobject
        (
            regionName,
            runTime.constant(),
            meshPath,
            runTime
        ),
        systemDictIO("mirrorMeshDict", args, runTime, regionName)
    );

    hexRef8Data refData
    (
        IOobject
        (
            "dummy",
            mesh.facesInstance(),
            polyMesh::meshSubDir,
            mesh,
            IOobject::READ_IF_PRESENT,
            IOobject::NO_WRITE,
            false
        )
    );

    if (!overwrite)
    {
        runTime++;
        mesh.setInstance(runTime.name());
    }


    // Set the precision of the points data to 10
    IOstream::defaultPrecision(max(10u, IOstream::defaultPrecision()));

    // Generate the mirrored mesh
    const fvMesh& mMesh = mesh.mirrorMesh();

    const_cast<fvMesh&>(mMesh).setInstance(mesh.facesInstance());
    Info<< "Writing mirrored mesh" << endl;
    mMesh.write();

    // Map the hexRef8 data
    polyTopoChangeMap map
    (
        mesh,
        mesh.nPoints(),         // nOldPoints,
        mesh.nFaces(),          // nOldFaces,
        mesh.nCells(),          // nOldCells,
        move(mesh.pointMap()),  // pointMap,
        List<objectMap>(0),     // pointsFromPoints,
        labelList(0),           // faceMap,
        List<objectMap>(0),     // facesFromFaces,
        move(mesh.cellMap()),   // cellMap,
        List<objectMap>(0),     // cellsFromCells,
        labelList(0),           // reversePointMap,
        labelList(0),           // reverseFaceMap,
        labelList(0),           // reverseCellMap,
        labelHashSet(0),        // flipFaceFlux,
        labelListList(0),       // patchPointMap,
        labelList(0),           // oldPatchSizes,
        labelList(0),           // oldPatchStarts,
        labelList(0),           // oldPatchNMeshPoints,
        autoPtr<scalarField>()  // oldCellVolumesPtr
    );
    refData.topoChange(map);
    refData.write();

    Info<< "End" << endl;

    return 0;
}


// ************************************************************************* //
