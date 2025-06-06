/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2015-2024 OpenFOAM Foundation
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
    Foam::ReynoldsStress

Description
    Reynolds-stress turbulence model base class

SourceFiles
    ReynoldsStress.C

\*---------------------------------------------------------------------------*/

#ifndef ReynoldsStress_H
#define ReynoldsStress_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class ReynoldsStress Declaration
\*---------------------------------------------------------------------------*/

template<class BasicMomentumTransportModel>
class ReynoldsStress
:
    public BasicMomentumTransportModel
{
    // Private member functions

        //- Return the source term for the momentum equation
        template<class RhoFieldType>
        tmp<fvVectorMatrix> DivDevTau
        (
            const RhoFieldType& rho,
            volVectorField& U
        ) const;


protected:

    // Protected data

        // Model coefficients

            dimensionedScalar couplingFactor_;

        // Fields

            volSymmTensorField R_;
            volScalarField nut_;


    // Protected Member Functions

        void boundNormalStress(volSymmTensorField& R) const;
        void correctWallShearStress(volSymmTensorField& R) const;

        //- Update the eddy-viscosity
        virtual void correctNut() = 0;

        //- Source term for the R equation
        virtual tmp<fvSymmTensorMatrix> RSource() const;

        //- Return the effective Reynolds stress flux
        tmp<surfaceVectorField> Refff(const volVectorField& U) const;


public:

    typedef typename BasicMomentumTransportModel::alphaField alphaField;
    typedef typename BasicMomentumTransportModel::rhoField rhoField;


    // Constructors

        //- Construct from components
        ReynoldsStress
        (
            const word& modelName,
            const alphaField& alpha,
            const rhoField& rho,
            const volVectorField& U,
            const surfaceScalarField& alphaRhoPhi,
            const surfaceScalarField& phi,
            const viscosity& viscosity
        );


    //- Destructor
    virtual ~ReynoldsStress()
    {}


    // Member Functions

        //- Re-read model coefficients if they have changed
        virtual bool read() = 0;

        //- Return the turbulence viscosity
        virtual tmp<volScalarField> nut() const
        {
            return nut_;
        }

        //- Return the turbulence viscosity on patch
        virtual tmp<scalarField> nut(const label patchi) const
        {
            return nut_.boundaryField()[patchi];
        }

        //- Return the turbulence kinetic energy
        virtual tmp<volScalarField> k() const;

        //- Return the Reynolds stress tensor [m^2/s^2]
        virtual tmp<volSymmTensorField> R() const;

        //- Return the effective surface stress
        virtual tmp<surfaceVectorField> devTau() const;

        //- Return the source term for the momentum equation
        virtual tmp<fvVectorMatrix> divDevTau(volVectorField& U) const;

        //- Return the source term for the momentum equation
        virtual tmp<fvVectorMatrix> divDevTau
        (
            const volScalarField& rho,
            volVectorField& U
        ) const;

        //- Validate the turbulence fields after construction
        //  Update turbulence viscosity and other derived fields as requires
        virtual void validate();

        //- Solve the turbulence equations and correct the turbulence viscosity
        virtual void correct() = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "ReynoldsStress.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
