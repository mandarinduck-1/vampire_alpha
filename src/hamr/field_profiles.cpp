//-----------------------------------------------------------------------------
//
// This source file is part of the VAMPIRE open source package under the
// GNU GPL (version 2) licence (see licence file for details).
//
// (c) Andrea Meo 2022.
//
// All rights reserved.
//
//-----------------------------------------------------------------------------

// C++ standard library headers
#include <iostream>
// #include "math.h"

// Vampire headers
#include "hamr.hpp"
#include "material.hpp"
#include "vmpi.hpp"

// hamr headers
#include "internal.hpp"

namespace hamr{
   namespace internal{

      //-----------------------------------------------------------------------------
      // Function to calculate the external field with trapezoidal temporal profile
      //-----------------------------------------------------------------------------
		double update_field_time_trapz_profile(const uint64_t current_time_step,//当前时间步
                                          const uint64_t rise_time_step,//磁场上升阶段的时间步数
                                          const uint64_t fall_time_step,//磁场下降阶段的时间步数
                                          const uint64_t bit_time_step//比特写入阶段的时间步数
                                          )
		//这个函数计算随时间变化的梯形轮廓磁场，用于模拟磁头写入时的磁场变化。
										  {

		   const double Hmax = hamr::internal::Hmax; // max field
		   const double Hmin = hamr::internal::Hmin;
			double H_inc = 0.0;

			// Determine max magnitude of external field during initial ramp
			if(current_time_step <= rise_time_step){
				H_inc = (Hmax-Hmin) / hamr::internal::H_rise_time * mp::dt_SI;
				//磁场从 Hmin 线性增加到 Hmax
			}
			// In the central region the max magnitude of the field is Hmax
			// else if(current_time_step > rise_time_step && current_time_step < bit_time_step-fall_time_step){
			// 	H_inc = 0.0;
			// }
			// Decrease field in the final region
			else if(current_time_step >= bit_time_step-fall_time_step && current_time_step <= bit_time_step){
				H_inc = -1.0*(Hmax-Hmin) / hamr::internal::H_fall_time * mp::dt_SI;
				//磁场从 Hmax 线性下降到 Hmin
			}

         return H_inc;
      } // end of trapezoidal profile


      //-----------------------------------------------------------------------------
      // Function to apply field only to atoms within box around centre of writer coil
      //-----------------------------------------------------------------------------
      void apply_field_spatial_box(const int start_index,
					                  const int end_index,
											const double Hloc_parity_field,
											//局部磁场的奇偶性（正负）
											const double Hvecx,
											const double Hvecy,
											const double Hvecz,
					                  std::vector<double>& x_total_external_field_array,
					                  std::vector<double>& y_total_external_field_array,
					                  std::vector<double>& z_total_external_field_array
      									)
		//这个函数将磁场应用到磁头附近特定空间区域内的原子上。
										{

			// Calculate constants for efficiency
			const double Hx_app = Hvecx*Hloc_parity_field;
			const double Hy_app = Hvecy*Hloc_parity_field;
			const double Hz_app = Hvecz*Hloc_parity_field;
			//计算有效磁场分量
			const double Hloc_min_x = hamr::internal::head_position_x - 0.5*hamr::internal::H_bounds_x - hamr::internal::NPS;  // Shift field box in downtrack of NPS
			const double Hloc_max_x = hamr::internal::head_position_x + 0.5*hamr::internal::H_bounds_x - hamr::internal::NPS;  // Shift field box in downtrack of NPS
			const double Hloc_min_y = hamr::internal::head_position_y - 0.5*hamr::internal::H_bounds_y;
			const double Hloc_max_y = hamr::internal::head_position_y + 0.5*hamr::internal::H_bounds_y;
            //定义磁场作用区域

			// Add localised applied field
			for(int atom=start_index;atom<end_index;atom++){
				const double cx = hamr::internal::atom_coords_x[atom];
				const double cy = hamr::internal::atom_coords_y[atom];

				// If atoms within field box, add contribution from external field
				if((cx >= Hloc_min_x) && (cx <= Hloc_max_x) && (cy >= Hloc_min_y) && (cy <= Hloc_max_y)){
					hamr::internal::x_field_array[atom] += Hx_app;
					hamr::internal::y_field_array[atom] += Hy_app;
					hamr::internal::z_field_array[atom] += Hz_app;
				}
				x_total_external_field_array[atom] += hamr::internal::x_field_array[atom];
				y_total_external_field_array[atom] += hamr::internal::y_field_array[atom];
				z_total_external_field_array[atom] += hamr::internal::z_field_array[atom];
			}
			//累加到总外场数组

			return;
      } // end of spatial box profile func



   } // end of namespace internal
} // end of namespace hamr
