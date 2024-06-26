/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2017-2024 OpenFOAM Foundation
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
    Foam::dummyIstream

Description
    Dummy stream for input. Aborts at any attempt to read from it.

SourceFiles

\*---------------------------------------------------------------------------*/

#ifndef dummyIstream_H
#define dummyIstream_H

#include "Istream.H"
//#include <sstream>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class dummyIstream Declaration
\*---------------------------------------------------------------------------*/

class dummyIstream
:
    public Istream
{

public:

    // Constructors

        //- Construct null
        dummyIstream()
        :
            Istream()
        {}


    //- Destructor
    ~dummyIstream()
    {}


    // Member Functions

        // Read functions

            //- Return next token from stream
            virtual Istream& read(token&)
            {
                NotImplemented;
                return *this;
            }

            //- Read a character
            virtual Istream& read(char&)
            {
                NotImplemented;
                return *this;
            }

            //- Read a word
            virtual Istream& read(word&)
            {
                NotImplemented;
                return *this;
            }

            // Read a string (including enclosing double-quotes)
            virtual Istream& read(string&)
            {
                NotImplemented;
                return *this;
            }

            //- Read an int32_t
            virtual Istream& read(int32_t&)
            {
                NotImplemented;
                return *this;
            }

            //- Read an int64_t
            virtual Istream& read(int64_t&)
            {
                NotImplemented;
                return *this;
            }

            //- Read a uint32_t
            virtual Istream& read(uint32_t&)
            {
                NotImplemented;
                return *this;
            }

            //- Read a uint64_t
            virtual Istream& read(uint64_t&)
            {
                NotImplemented;
                return *this;
            }

            //- Read a floatScalar
            virtual Istream& read(floatScalar&)
            {
                NotImplemented;
                return *this;
            }

            //- Read a doubleScalar
            virtual Istream& read(doubleScalar&)
            {
                NotImplemented;
                return *this;
            }

            //- Read a longDoubleScalar
            virtual Istream& read(longDoubleScalar&)
            {
                NotImplemented;
                return *this;
            }

            //- Read binary block
            virtual Istream& read(char*, std::streamsize)
            {
                NotImplemented;
                return *this;
            }

            //- Rewind and return the stream so that it may be read again
            virtual Istream& rewind()
            {
                NotImplemented;
                return *this;
            }

            //- Return flags of stream
            virtual ios_base::fmtflags flags() const
            {
                NotImplemented;
                return ios_base::fmtflags(0);
            }

            //- Set flags of stream
            virtual ios_base::fmtflags flags(const ios_base::fmtflags f)
            {
                NotImplemented;
                return f;
            }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
