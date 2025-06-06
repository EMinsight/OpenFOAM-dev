/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2024 OpenFOAM Foundation
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

Class
    Foam::MRFZoneList

Description
    List container for MRF zones

SourceFiles
    MRFZoneList.C

\*---------------------------------------------------------------------------*/

#ifndef MRFZoneList_H
#define MRFZoneList_H

#include "fvMesh.H"
#include "dictionary.H"
#include "fvMatricesFwd.H"
#include "MRFZone.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators
class MRFZoneList;
Ostream& operator<<(Ostream& os, const MRFZoneList& models);

/*---------------------------------------------------------------------------*\
                         Class MRFZoneList Declaration
\*---------------------------------------------------------------------------*/

class MRFZoneList
:
    public PtrList<MRFZone>
{
protected:

    // Protected data

        //- Reference to the mesh database
        const fvMesh& mesh_;


public:

    // Constructors

        MRFZoneList(const fvMesh& mesh, const dictionary& dict);

        //- Disallow default bitwise copy construction
        MRFZoneList(const MRFZoneList&) = delete;


    //- Destructor
    ~MRFZoneList();


    // Member Functions

        //- Reset the source list
        void reset(const dictionary& dict);

        //- Return the Coriolis acceleration
        tmp<volVectorField> DDt
        (
            const volVectorField& U
        ) const;

        //- Return the Coriolis acceleration
        tmp<volVectorField> DDt
        (
            const volScalarField& rho,
            const volVectorField& U
        ) const;

        //- Return the centrifugal acceleration
        tmp<volVectorField> centrifugalAcceleration() const;

        //- Make the given absolute velocity relative within the MRF region
        void makeRelative(volVectorField& U) const;

        //- Make the given absolute flux relative within the MRF region
        void makeRelative(surfaceScalarField& phi) const;

        //- Return the given absolute flux relative within the MRF region
        tmp<surfaceScalarField> relative
        (
            const tmp<surfaceScalarField>& phi
        ) const;

        //- Return the given absolute boundary flux relative within
        //  the MRF region
        tmp<FieldField<surfaceMesh::PatchField, scalar>> relative
        (
            const tmp<FieldField<surfaceMesh::PatchField, scalar>>&
        ) const;

        //- Return the given absolute patch flux relative within
        //  the MRF region
        tmp<Field<scalar>> relative
        (
            const tmp<Field<scalar>>& tphi,
            const label patchi
        ) const;

        //- Make the given absolute mass-flux relative within the MRF region
        void makeRelative
        (
            const surfaceScalarField& rho,
            surfaceScalarField& phi
        ) const;

        //- Make the given relative velocity absolute within the MRF region
        void makeAbsolute(volVectorField& U) const;

        //- Make the given relative flux absolute within the MRF region
        void makeAbsolute(surfaceScalarField& phi) const;

        //- Return the given relative flux absolute within the MRF region
        tmp<surfaceScalarField> absolute
        (
            const tmp<surfaceScalarField>& phi
        ) const;

        //- Make the given relative mass-flux absolute within the MRF region
        void makeAbsolute
        (
            const surfaceScalarField& rho,
            surfaceScalarField& phi
        ) const;

        //- Return the given relative flux absolute within the MRF region
        tmp<surfaceScalarField> absolute
        (
            const tmp<surfaceScalarField>& phi,
            const volScalarField& rho
        ) const;

        void makeAbsolute(Field<vector>& Up, const label patchi) const;

        //- Update MRFZone faces if the mesh topology changes
        void update();


        // I-O

            //- Read dictionary
            bool read(const dictionary& dict);


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const MRFZoneList&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
