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

#include "CloudFunctionObjectList.H"
#include "functionObject.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::CloudFunctionObjectList<CloudType>::CloudFunctionObjectList
(
    CloudType& owner
)
:
    PtrList<CloudFunctionObject<CloudType>>(),
    owner_(owner),
    dict_(dictionary::null)
{}


template<class CloudType>
Foam::CloudFunctionObjectList<CloudType>::CloudFunctionObjectList
(
    CloudType& owner,
    const dictionary& dict
)
:
    PtrList<CloudFunctionObject<CloudType>>(),
    owner_(owner),
    dict_(dict)
{
    if (!functionObject::postProcess)
    {
        wordList modelNames(dict.toc());

        Info<< "Constructing cloud functions" << endl;

        if (modelNames.size() > 0)
        {
            this->setSize(modelNames.size());

            forAll(modelNames, i)
            {
                const word& modelName = modelNames[i];

                const dictionary& modelDict(dict.subDict(modelName));

                // read the type of the function object
                const word objectType(modelDict.lookup("type"));

                this->set
                (
                    i,
                    CloudFunctionObject<CloudType>::New
                    (
                        modelDict,
                        owner,
                        objectType,
                        modelName
                    )
                );
            }
        }
        else
        {
            Info<< "    none" << endl;
        }
    }
    else
    {
        Info<< "Not constructing cloud functions" << endl;
    }
}


template<class CloudType>
Foam::CloudFunctionObjectList<CloudType>::CloudFunctionObjectList
(
    const CloudFunctionObjectList& cfol
)
:
    PtrList<CloudFunctionObject<CloudType>>(cfol),
    owner_(cfol.owner_),
    dict_(cfol.dict_)
{}


// * * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::CloudFunctionObjectList<CloudType>::~CloudFunctionObjectList()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void Foam::CloudFunctionObjectList<CloudType>::preEvolve()
{
    forAll(*this, i)
    {
        this->operator[](i).preEvolve();
    }
}


template<class CloudType>
void Foam::CloudFunctionObjectList<CloudType>::postEvolve()
{
    forAll(*this, i)
    {
        this->operator[](i).postEvolve();
    }
}


template<class CloudType>
void Foam::CloudFunctionObjectList<CloudType>::postMove
(
    typename CloudType::parcelType& p,
    const scalar dt,
    const point& position0,
    bool& keepParticle
)
{
    forAll(*this, i)
    {
        this->operator[](i).postMove(p, dt, position0, keepParticle);
    }
}


template<class CloudType>
void Foam::CloudFunctionObjectList<CloudType>::postPatch
(
    const typename CloudType::parcelType& p,
    const polyPatch& pp
)
{
    forAll(*this, i)
    {
        this->operator[](i).postPatch(p, pp);
    }
}


template<class CloudType>
void Foam::CloudFunctionObjectList<CloudType>::preFace
(
    const typename CloudType::parcelType& p
)
{
    forAll(*this, i)
    {
        this->operator[](i).preFace(p);
    }
}


template<class CloudType>
void Foam::CloudFunctionObjectList<CloudType>::postFace
(
    const typename CloudType::parcelType& p
)
{
    forAll(*this, i)
    {
        this->operator[](i).postFace(p);
    }
}


// ************************************************************************* //
