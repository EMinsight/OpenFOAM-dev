/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2013-2025 OpenFOAM Foundation
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
    Foam::functionObjects::turbulenceFields

Description
    Stores derived turbulence fields on the mesh database for further
    manipulation.

Usage
    \table
        Property     | Description                 | Required | Default value
        type         | Type name: processorField   | yes      |
        fields       | Fields to store (see below) | yes      |
        prefix       | If true prefix fields       | no       | no
        phase        | phase name                  | no       |
    \endtable

    Where \c fields can include:
    \plaintable
        k           | turbulence kinetic energy
        epsilon     | turbulence kinetic energy dissipation rate
        omega       | turbulence specific dissipation rate
        nut         | turbulence viscosity (incompressible)
        nuEff       | effective turbulence viscosity (incompressible)
        nut         | turbulence viscosity (compressible)
        nuEff       | effective turbulence viscosity (compressible)
        kappaEff    | effective turbulence thermal diffusivity (compressible)
        R           | Reynolds stress tensor
    \endplaintable

    If the optional \c prefix entry is set true the turbulence fields are stored
    with the prefix "momentumTransportModel:", e.g.:
    \verbatim
    momentumTransportModel:R
    \endverbatim

    Example of function object specification:
    \verbatim
    turbulenceFields1
    {
        type        turbulenceFields;
        libs        ("libfieldFunctionObjects.so");
        ...
        fields
        (
            R
            devTau
        );
    }
    \endverbatim

See also
    Foam::functionObjects::fvMeshFunctionObject
    Foam::functionObjects::timeControl

SourceFiles
    turbulenceFields.C

\*---------------------------------------------------------------------------*/

#ifndef turbulenceFields_functionObject_H
#define turbulenceFields_functionObject_H

#include "fvMeshFunctionObject.H"
#include "HashSet.H"
#include "NamedEnum.H"
#include "volFieldsFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{

/*---------------------------------------------------------------------------*\
                      Class turbulenceFields Declaration
\*---------------------------------------------------------------------------*/

class turbulenceFields
:
    public fvMeshFunctionObject
{
public:

    enum class compressibleField
    {
        k,
        epsilon,
        omega,
        nut,
        nuEff,
        kappaEff,
        R
    };
    static const NamedEnum<compressibleField, 7> compressibleFieldNames_;

    enum class incompressibleField
    {
        k,
        epsilon,
        omega,
        nut,
        nuEff,
        R
    };
    static const NamedEnum<incompressibleField, 6> incompressibleFieldNames_;


protected:

    // Protected data

        //- Fields to load
        wordHashSet fieldSet_;

        //- Optional field prefix to avoid name clashes
        //  Defaults to null
        word prefix_;

        //- Optional phase name
        word phaseName_;


    // Protected Member Functions

        word modelName();

        //- Process the turbulence field
        template<class Type>
        void processField
        (
            const word& fieldName,
            const tmp<VolField<Type>>& tvalue
        );


public:

    //- Runtime type information
    TypeName("turbulenceFields");


    // Constructors

        //- Construct from Time and dictionary
        turbulenceFields
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );

        //- Disallow default bitwise copy construction
        turbulenceFields(const turbulenceFields&) = delete;


    //- Destructor
    virtual ~turbulenceFields();


    // Member Functions

        //- Read the controls
        virtual bool read(const dictionary&);

        //- Return the list of fields required
        virtual wordList fields() const
        {
            return wordList::null();
        }

        //- Calculate turbulence fields
        virtual bool execute();

        //- Write the turbulence fields
        virtual bool write();


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const turbulenceFields&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "turbulenceFieldsTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
