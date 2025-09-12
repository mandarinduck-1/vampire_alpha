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
//

// C++ standard library headers
#include <string>
#include <sstream>
#include <vector>

// Vampire headers
#include "cells.hpp"
#include "errors.hpp"
#include "vio.hpp"
#include "units.hpp"
#include "sim.hpp"
#include "internal.hpp"
#include "atoms.hpp" // 新增，便于直接对原子外场赋值

namespace cells
{

   //---------------------------------------------------------------------------
   // Function to process input file parameters for cells module
   //---------------------------------------------------------------------------
   bool match_input_parameter(std::string const key, std::string const word, std::string const value, std::string const unit, int const line)
   {
      // cells模块参数解析
      std::string prefix = "cells";
      if (key == prefix)
      {
         // 处理cells相关参数
         std::string test = "macro-cell-size";
         if (word == test)
         {
            double csize = atof(value.c_str());
            vin::check_for_valid_value(csize, word, line, prefix, unit, "length", 0.0, 1.0e7, "input", "0.0 Angstroms - 1 millimetre");
            cells::macro_cell_size = csize;
            cells::macro_cell_size_x = csize;
            cells::macro_cell_size_y = csize;
            cells::macro_cell_size_z = csize;
            return true;
         }
         test = "macro-cell-size-x";
         if (word == test)
         {
            double csize = atof(value.c_str());
            vin::check_for_valid_value(csize, word, line, prefix, unit, "length", 0.0, 1.0e7, "input", "0.0 Angstroms - 1 millimetre");
            cells::macro_cell_size = csize;
            cells::macro_cell_size_x = csize;
            return true;
         }
         test = "macro-cell-size-y";
         if (word == test)
         {
            double csize = atof(value.c_str());
            vin::check_for_valid_value(csize, word, line, prefix, unit, "length", 0.0, 1.0e7, "input", "0.0 Angstroms - 1 millimetre");
            cells::macro_cell_size_y = csize;
            return true;
         }
         test = "macro-cell-size-z";
         if (word == test)
         {
            double csize = atof(value.c_str());
            vin::check_for_valid_value(csize, word, line, prefix, unit, "length", 0.0, 1.0e7, "input", "0.0 Angstroms - 1 millimetre");
            cells::macro_cell_size_z = csize;
            return true;
         }

         // 以下是局部场实现方法
         if (word == "local_field_num_regions")
         {
            int num = atoi(value.c_str());
            cells::g_num_local_field_regions = num;
            sim::local_applied_field = true;
            // std::cout << "[局部场] 解析到区域数: " << num << std::endl;
            return true;
         }
         if (word.find("local_field_region_") == 0)
         {
            size_t idx1 = std::string("local_field_region_").size();
            size_t idx2 = word.find('_', idx1);
            int region_idx = std::stoi(word.substr(idx1, idx2 - idx1)) - 1; // 0-based
            std::string param = word.substr(idx2 + 1);

            if (region_idx >= (int)cells::g_local_field_regions.size())
               cells::g_local_field_regions.resize(region_idx + 1);

            cells::LocalFieldRegion &region = cells::g_local_field_regions[region_idx];

            double val = atof(value.c_str());
            std::string type;

            // 日志输出
            // std::cout << "[局部场] 区域" << region_idx+1 << " 参数: " << param << " = " << val << " " << unit << std::endl;

            if (param == "material_type")
               region.material_type = (int)val;
            else if (param == "x_min")
            {
               units::convert(unit, val, type);
               region.x_min = val;
            }
            else if (param == "x_max")
            {
               units::convert(unit, val, type);
               region.x_max = val;
            }
            else if (param == "y_min")
            {
               units::convert(unit, val, type);
               region.y_min = val;
            }
            else if (param == "y_max")
            {
               units::convert(unit, val, type);
               region.y_max = val;
            }
            else if (param == "z_min")
            {
               units::convert(unit, val, type);
               region.z_min = val;
            }
            else if (param == "z_max")
            {
               units::convert(unit, val, type);
               region.z_max = val;
            }
            else if (param == "field_x")
            {
               units::convert(unit, val, type);
               region.field_x = val;
            }
            else if (param == "field_y")
            {
               units::convert(unit, val, type);
               region.field_y = val;
            }
            else if (param == "field_z")
            {
               units::convert(unit, val, type);
               region.field_z = val;
            }
            sim::local_applied_field = true;
            return true;
         }
         // 未匹配到关键词
         return false;
      }

      if (key.find("local_field_region_") == 0)
      {
         sim::local_applied_field = true;
         size_t idx1 = std::string("local_field_region_").size();
         size_t idx2 = key.find('_', idx1);
         int region_idx = std::stoi(key.substr(idx1, idx2 - idx1)) - 1;
         std::string param = key.substr(idx2 + 1);

         if (region_idx >= (int)cells::g_local_field_regions.size())
            cells::g_local_field_regions.resize(region_idx + 1);

         cells::LocalFieldRegion &region = cells::g_local_field_regions[region_idx];

         double val = std::stod(value);
         std::string type;

         if (param == "material_type")
            region.material_type = (int)val;
         else if (param == "x_min")
         {
            units::convert(unit, val, type);
            region.x_min = val;
         }
         else if (param == "x_max")
         {
            units::convert(unit, val, type);
            region.x_max = val;
         }
         else if (param == "y_min")
         {
            units::convert(unit, val, type);
            region.y_min = val;
         }
         else if (param == "y_max")
         {
            units::convert(unit, val, type);
            region.y_max = val;
         }
         else if (param == "z_min")
         {
            units::convert(unit, val, type);
            region.z_min = val;
         }
         else if (param == "z_max")
         {
            units::convert(unit, val, type);
            region.z_max = val;
         }
         else if (param == "field_x")
         {
            units::convert(unit, val, type);
            region.field_x = val;
         }
         else if (param == "field_y")
         {
            units::convert(unit, val, type);
            region.field_y = val;
         }
         else if (param == "field_z")
         {
            units::convert(unit, val, type);
            region.field_z = val;
         }
         return true;
      }
      if (key == "local_field_num_regions")
      {
         sim::local_applied_field = true;
         cells::g_num_local_field_regions = std::stoi(value);
         return true;
      }
      return false;
   }

   // 局部场区域读取函数
   std::vector<LocalFieldRegion> read_local_field_regions_from_input()
   {
      if ((int)cells::g_local_field_regions.size() > cells::g_num_local_field_regions)
         cells::g_local_field_regions.resize(cells::g_num_local_field_regions);
      return cells::g_local_field_regions;
   }

   // 局部场应用函数（直接对原子外场赋值）
   void apply_local_field(
       const std::vector<LocalFieldRegion> &regions,
       const std::vector<int> &atom_type_array,
       const std::vector<double> &atom_coords_x,
       const std::vector<double> &atom_coords_y,
       const std::vector<double> &atom_coords_z,
       const std::vector<int> &atom_cell_id_array,
       int num_atoms)
   {
      // std::cout << "[DEBUG] apply_local_field called" << std::endl; // ★加在这里

      // 始终初始化局部场数组，即使没有局部场区域
      local_field_array_x.assign(num_atoms, 0.0);
      local_field_array_y.assign(num_atoms, 0.0);
      local_field_array_z.assign(num_atoms, 0.0);

      if (sim::local_applied_field && !regions.empty())
      {
         for (const auto &region : regions)
         {
            /*  std::cout << "[局部场] 应用区域: material_type=" << region.material_type
                        << " x=[" << region.x_min << "," << region.x_max << "]"
                        << " y=[" << region.y_min << "," << region.y_max << "]"
                        << " z=[" << region.z_min << "," << region.z_max << "]"
                        << " field=(" << region.field_x << "," << region.field_y << "," << region.field_z << ")" << std::endl;*/
            for (int i = 0; i < num_atoms; i++)
            {
               if (i < (int)atom_type_array.size() &&
                   i < (int)atom_coords_x.size() &&
                   i < (int)atom_coords_y.size() &&
                   i < (int)atom_coords_z.size() &&
                   atom_type_array[i] == region.material_type &&
                   atom_coords_x[i] >= region.x_min && atom_coords_x[i] <= region.x_max &&
                   atom_coords_y[i] >= region.y_min && atom_coords_y[i] <= region.y_max &&
                   atom_coords_z[i] >= region.z_min && atom_coords_z[i] <= region.z_max)
               {
                  local_field_array_x[i] += region.field_x;
                  local_field_array_y[i] += region.field_y;
                  local_field_array_z[i] += region.field_z;
                  /*std::cout << "[局部场] 原子" << i << " 分配 field=("
                             << region.field_x << "," << region.field_y << "," << region.field_z << ")" << std::endl;*/
               }
            }
         }

         /* for(int i=0; i<10 && i<num_atoms; ++i) {
             std::cout << "[调试] Atom " << i << " local_field_x = " << local_field_array_x[i] << std::endl;
            }

           for(int i=0; i<10 && i<num_atoms; ++i) {
                std::cout << "[调试] Atom " << i
                << " type=" << atom_type_array[i]
                << " x=" << atom_coords_x[i]
                << " y=" << atom_coords_y[i]
                << " z=" << atom_coords_z[i]
                << " local_field_x = " << local_field_array_x[i] << std::endl;
           }
          //这一部分是在调试的时候加上的，为了判断参数调用在哪一步终止
         */
      }
   }

   //---------------------------------------------------------------------------
   // Function to process material parameters
   //---------------------------------------------------------------------------
   bool match_material_parameter(std::string const word, std::string const value, std::string const unit, int const line, int const super_index, const int sub_index)
   {
      std::string prefix = "material:";
      return false;
   }

} // end of cells namespace