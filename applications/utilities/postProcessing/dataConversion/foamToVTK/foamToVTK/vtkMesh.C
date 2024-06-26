/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2024 OpenFOAM Foundation
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

#include "vtkMesh.H"
#include "fvMeshSubset.H"
#include "Time.H"
#include "cellSet.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::vtkMesh::vtkMesh
(
    fvMesh& baseMesh,
    const vtkTopo::vtkPolyhedra polyhedra,
    const word& setName
)
:
    baseMesh_(baseMesh),
    subsetter_(baseMesh),
    polyhedra_(polyhedra),
    setName_(setName)
{
    if (setName != word::null)
    {
        // Read cellSet using whole mesh
        cellSet currentSet(baseMesh_, setName_);

        // Set current subset
        subsetter_.setLargeCellSubset(currentSet);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::fvMesh::readUpdateState Foam::vtkMesh::readUpdate()
{
    fvMesh::readUpdateState meshState = baseMesh_.readUpdate();

    if (meshState != fvMesh::UNCHANGED)
    {
        // Note: since fvMeshSubset has no setPoints() functionality,
        // reconstruct the subset even if only movement.

        topoPtr_.clear();

        if (setName_.size())
        {
            Info<< "Subsetting mesh based on cellSet " << setName_ << endl;

            // Read cellSet using whole mesh
            cellSet currentSet(baseMesh_, setName_);

            subsetter_.setLargeCellSubset(currentSet);
        }
    }

    return meshState;
}


// ************************************************************************* //
