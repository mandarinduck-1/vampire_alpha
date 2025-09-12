#ifndef CUDA_H_
#define CUDA_H_
//-----------------------------------------------------------------------------
//
// This header file is part of the VAMPIRE open source package under the
// GNU GPL (version 2) licence (see licence file for details).
//
// (c) R F L Evans 2015. All rights reserved.
//
//-----------------------------------------------------------------------------

namespace vcuda{

#ifdef CUDA
//这意味着只有使用了CUDA宏的时候才会启用下面的代码

   //-----------------------------------------------------------------------------
   // Variables used for cuda GPU acceleration
   //-----------------------------------------------------------------------------

   //-----------------------------------------------------------------------------
   // Functions for cuda GPU acceleration
   //-----------------------------------------------------------------------------
   extern bool initialize(bool cpu_stats);
   extern bool initialize_dipole();
   extern void llg_heun();  //这是自旋动力学核心算法
   extern void stats_update();
   extern void finalize();
   extern void transfer_spin_positions_from_gpu_to_cpu(); //从GPU拷贝自旋位置到CPU
   extern void transfer_dipole_fields_from_cpu_to_gpu(); //从CPU拷贝偶极场到GPU
   extern void transfer_dipole_cells_fields_from_gpu_to_cpu(); //
   extern void update_dipolar_fields();
   extern void mc_step(); //在GPU上执行的MC步

   namespace config{
      extern void synchronise();
   }

   namespace stats
   {
      void update ();
      void get ();
      void reset ();
   } /* stats */

#endif

} // end of vcuda namespace

#endif //CUDA_H_
