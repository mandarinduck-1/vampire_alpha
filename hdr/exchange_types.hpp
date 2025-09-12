//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Richard F L Evans 2018. All rights reserved.
//
//   Email: richard.evans@york.ac.uk
//
//------------------------------------------------------------------------------
//

#ifndef EXCHANGE_TYPES_H_
#define EXCHANGE_TYPES_H_

//--------------------------------------------------------------------------------
// Namespace for variables and functions for exchange module
//--------------------------------------------------------------------------------
namespace exchange{

   //---------------------------------------------------------------------------
   // Enumerated list of available exchange types
   //---------------------------------------------------------------------------
   enum exchange_t { isotropic = 0, // isotropic exchange interactions
                     // J是个标量
                     vectorial = 1, // vector exchange Jxx, Jyy, Jzz
                     // J是个二阶张量但是只有对角元
                     tensorial = 2 // tensor exchange Jxx, Jxy ... Jzz
                     // J是个有非对角元的二阶张量
   };

}

#endif //EXCHANGE_TYPES_H_
