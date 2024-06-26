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

\*---------------------------------------------------------------------------*/

#include "uniformJumpFvPatchField.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Type>
Foam::scalar Foam::uniformJumpFvPatchField<Type>::t() const
{
    return this->db().time().userTimeValue();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
Foam::uniformJumpFvPatchField<Type>::uniformJumpFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    fixedJumpFvPatchField<Type>(p, iF, dict, false),
    jumpTable_()
{
    if (this->cyclicPatch().owner())
    {
        jumpTable_ = Function1<Type>::New("jumpTable", dict);

        this->jumpRef() = Field<Type>(p.size(), jumpTable_->value(t()));
    }

    this->evaluateNoUpdateCoeffs();
}


template<class Type>
Foam::uniformJumpFvPatchField<Type>::uniformJumpFvPatchField
(
    const uniformJumpFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fieldMapper& mapper
)
:
    fixedJumpFvPatchField<Type>(ptf, p, iF, mapper),
    jumpTable_(ptf.jumpTable_, false)
{}


template<class Type>
Foam::uniformJumpFvPatchField<Type>::uniformJumpFvPatchField
(
    const uniformJumpFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    fixedJumpFvPatchField<Type>(ptf, iF),
    jumpTable_(ptf.jumpTable_, false)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void Foam::uniformJumpFvPatchField<Type>::updateCoeffs()
{
    if (this->updated())
    {
        return;
    }

    if (this->cyclicPatch().owner())
    {
        this->jumpRef() = jumpTable_->value(t());
    }

    fixedJumpFvPatchField<Type>::updateCoeffs();
}


template<class Type>
void Foam::uniformJumpFvPatchField<Type>::write(Ostream& os) const
{
    fvPatchField<Type>::write(os);

    if (this->cyclicPatch().owner())
    {
        writeEntry(os, jumpTable_());
    }
}


// ************************************************************************* //
