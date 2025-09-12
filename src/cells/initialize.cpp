//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Andrea Meo 2016. All rights reserved.
//
//   Email: am1808@york.ac.uk
//
//------------------------------------------------------------------------------

// C++ standard library headers
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

// Vampire headers
#include "cells.hpp"
#include "material.hpp"
#include "errors.hpp"
#include "vio.hpp"
#include "vmpi.hpp"
#include "atoms.hpp"

// cells module headers
#include "internal.hpp"

namespace cells
{

   //----------------------------------------------------------------------------
   // Function to initialize cells module
   //----------------------------------------------------------------------------
   void initialize(const double system_dimensions_x,
                   const double system_dimensions_y,
                   const double system_dimensions_z,
                   const double unit_cell_size_x,
                   const double unit_cell_size_y,
                   const double unit_cell_size_z,
                   const std::vector<double> &atom_coords_x,
                   const std::vector<double> &atom_coords_y,
                   const std::vector<double> &atom_coords_z,
                   const std::vector<int> &atom_type_array,
                   const std::vector<int> &atom_cell_id_array,
                   const int num_total_atoms_for_dipole,
                   const int num_atoms)
   {

      //-------------------------------------------------------------------------------------
      // Check for cells calculation enabled, if not do nothing
      //-------------------------------------------------------------------------------------
      /*      if(!cells::internal::enabled) return;

             // output informative message
             zlog << zTs() << "Initialising data structures for macro-cell calculation." << std::endl;

             // check for prior initialisation
             if(cells::internal::initialised){
                zlog << zTs() << "Warning: Localised temperature calculation already initialised. Continuing." << std::endl;
                return;
             }
      */
      // check calling of routine if error checking is activated
      if (err::check == true)
         ;

      //-------------------------------------------------------------------------------------
      // Define variable needed for mag() function
      //-------------------------------------------------------------------------------------

      //
      cells::internal::num_atoms = num_atoms;
      cells::internal::atom_type_array = atom_type_array;
      cells::atom_cell_id_array = atom_cell_id_array;
      // 为了完成局部场才加的

      //-------------------------------------------------------------------------------------
      // Calculate number of microcells
      //-------------------------------------------------------------------------------------
      // 这里假设 macro_cell_size_x/y/z 已在 cells.hpp 里声明为 extern double 并在某 cpp 文件定义
      unsigned int dx = static_cast<unsigned int>(ceil((system_dimensions_x + 0.01) / cells::macro_cell_size_x));
      unsigned int dy = static_cast<unsigned int>(ceil((system_dimensions_y + 0.01) / cells::macro_cell_size_y));
      unsigned int dz = static_cast<unsigned int>(ceil((system_dimensions_z + 0.01) / cells::macro_cell_size_z));

      cells::num_cells = dx * dy * dz;
      cells::internal::cell_position_array.resize(3 * cells::num_cells);

      // 初始化 pos_and_mom_array (CUDA需要这个数组)
      cells::pos_and_mom_array.resize(4 * cells::num_cells, 0.0);

      // 计算每个cell的位置和磁矩
      int cell_index = 0;
      for (unsigned int k = 0; k < dz; k++)
      {
         for (unsigned int j = 0; j < dy; j++)
         {
            for (unsigned int i = 0; i < dx; i++)
            {
               // 计算cell的中心位置
               double cx = (i + 0.5) * cells::macro_cell_size_x;
               double cy = (j + 0.5) * cells::macro_cell_size_y;
               double cz = (k + 0.5) * cells::macro_cell_size_z;

               // 存储位置信息
               cells::pos_and_mom_array[4 * cell_index + 0] = cx;
               cells::pos_and_mom_array[4 * cell_index + 1] = cy;
               cells::pos_and_mom_array[4 * cell_index + 2] = cz;
               cells::pos_and_mom_array[4 * cell_index + 3] = 0.0; // 初始磁矩为0

               cell_index++;
            }
         }
      }

      //------------------ 局部场初始化与应用 ------------------
      // std::cout << "[局部场] 开始读取局部场参数..." << std::endl;
      std::vector<cells::LocalFieldRegion> regions = cells::read_local_field_regions_from_input();
      // std::cout << "[局部场] 读取到 " << regions.size() << " 个区域" << std::endl;
      cells::apply_local_field(
          regions,
          atom_type_array,
          atom_coords_x,
          atom_coords_y,
          atom_coords_z,
          atom_cell_id_array,
          num_atoms);
      // std::cout << "[局部场] apply_local_field 调用完成" << std::endl;
      //------------------ 局部场初始化与应用 ------------------
   }

} // end of cells namespace