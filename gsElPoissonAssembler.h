/** @file gsElPoissonAssembler.h

   @brief Provides assembler for the Poisson equation with correctly working penalization,
   as well as some extra functionality used in Thermo-Elasticity simulations

   This file is part of the G+Smo library.

   This Source Code Form is subject to the terms of the Mozilla Public
   License, v. 2.0. If a copy of the MPL was not distributed with this
   file, You can obtain one at http://mozilla.org/MPL/2.0/.

   Author(s): A. Shamanskiy
*/

#pragma once

#include <gsAssembler/gsPoissonAssembler.h>
#include <fstream>



namespace gismo
{

/** @brief
   Thermo-elasticity module utilizes the Poisson Assembler to solve
   the heat equation. Since the incoming heat data from Navier-Stokes solver
   are to be set repeatedly, the penalization Dirichlet strategy was chosen.
   However, the standard Poisson solver has a minor bug in the penalizeDirichletDofs()
   method. This version corrects the bug and add extra possibilities of setting Dirichlet
   DOFs coming from the outside.

*/
template <class T>
class gsElPoissonAssembler : public gsPoissonAssembler<T>
{
public:

    gsElPoissonAssembler(const gsMultiPatch<T> & patches,
                         const gsMultiBasis<T> & bases,
                         const gsBoundaryConditions<T> & bcInfo,
                         const gsFunction<T> & force,
                         T conductivity = 1.,
                         dirichlet::strategy dirStrategy = dirichlet::elimination,
                         iFace::strategy intStrategy = iFace::glue);

   void assemble();

   void penalizeDirichletDofs();

   /// Should only be used with penalization Dirichlet enforcement strategy
   void setDirichletDoFs(const gsMatrix<> & ddofs, int targetPatch, const boxSide & targetSide);

   void setDirichletDoFs(const gsMultiPatch<> & sourceGeometry,
                         const gsMultiPatch<> & sourceSolution,
                         int sourcePatch, const boxSide & sourceSide,
                         int targetPatch, const boxSide & targetSide);

   void setDirichletDoFs(const gsField<> & sourceField,
                         int sourcePatch, const boxSide & sourceSide,
                         int targetPatch, const boxSide & targetSide);

   void setUnitingConstraint(const boxSide & side, bool verbosity = false);

protected:

   /// Check if two boundaries have at least the same starting and ending points
   /// Returns 1 if they match, return -1 if they match,
   /// but parameterization directions are opposite,
   /// return 0 if ends don't match
   int checkMatchingBoundaries(const gsGeometry<> & sourceBoundary,
                               const gsGeometry<> & targetBoundary);

protected:

   // Members from gsAssembler
   using gsPoissonAssembler<T>::m_pde_ptr;
   using gsPoissonAssembler<T>::m_bases;
   using gsPoissonAssembler<T>::m_ddof;
   using gsPoissonAssembler<T>::m_options;
   using gsPoissonAssembler<T>::m_system;

   const T PP = 1e9; // magic number
};

template <class T>
void gsWriteVector(const gsMatrix<T> & vector,const std::string& fname)
{
    std::ofstream ofs;
    ofs.open(fname.c_str(),std::ofstream::out);
    ofs << vector;
    ofs.close();
}

template <class T>
bool gsReadVector(gsMatrix<T> & vector,const std::string& fname)
{
    std::vector<T> inputVector;
    std::ifstream input(fname.c_str());
    bool result = false;
    if (input)
    {
        result = true;
        T value;
        while (input >> value)
        {
            inputVector.push_back(value);
        }
        vector.setZero(inputVector.size(),1);
        for (int i = 0; i < inputVector.size(); ++i)
        {
            vector(i,0) = inputVector[i];
        }
    }

    return result;
}

} // namespace gismo


#ifndef GISMO_BUILD_LIB
#include GISMO_HPP_HEADER(gsPoissonAssembler.hpp)
#endif
