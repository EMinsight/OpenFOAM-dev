/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2023-2024 OpenFOAM Foundation
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

#include "includeFvModelEntry.H"
#include "addToMemberFunctionSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionEntries
{
    defineTypeNameAndDebug(includeFvModelEntry, 0);

    addToMemberFunctionSelectionTable
    (
        functionEntry,
        includeFvModelEntry,
        execute,
        dictionaryIstream
    );
}
}


Foam::fileName Foam::functionEntries::includeFvModelEntry::fvModelDictPath
(
    "caseDicts/fvModels"
);


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::functionEntries::includeFvModelEntry::execute
(
    dictionary& parentDict,
    Istream& is
)
{
    // Read line containing the function name and the optional arguments
    const Tuple2<string, label> fNameArgs(readFuncNameArgs(is));

    return readConfigFile
    (
        "model",
        fNameArgs,
        parentDict,
        fvModelDictPath,
        "constant"
    );
}


// ************************************************************************* //
