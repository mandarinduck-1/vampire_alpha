//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Andrea Meo and Richard Evans 2019. All rights reserved.
//
//   Email: richard.evans@york.ac.uk
//
//------------------------------------------------------------------------------
//

// Standard Libraries
#include <cstdlib>

// Vampire Header files
#include "vmath.hpp"
#include "errors.hpp"
#include "sim.hpp"
#include "stats.hpp"
#include "vio.hpp"

#include "cells.hpp" //为实现局部场加的代码，只在调试的时候生效//

namespace program
{

	/// @brief Function to calculate the hysteresis loop
	///
	/// @callgraph
	/// @callergraph
	///
	/// @details Consists of a sequence of sub-calculations of fixed temperature. The system is initialised
	/// ordered. After initialisation a whole hysteresis loop of the system and coercivity are calculated.
	///
	/// @section License
	/// Use of this code, either in source or compiled form, is subject to license from the authors.
	/// Copyright \htmlonly &copy \endhtmlonly Richard Evans, 2009-2010. All Rights Reserved.
	///
	/// @section Information
	/// @author  Weijia Fan, wf507@york.ac.uk
	/// @version 1.0
	/// @date    27/01/2010
	///
	/// @return EXIT_SUCCESS
	///
	/// @internal
	///	Created:		27/01/2010
	///	Revision:	  ---
	///=====================================================================================
	///
	int hysteresis()
	{

		// check calling of routine if error checking is activated
		if (err::check == true)
		{
			std::cout << "program::hysteresis has been called" << std::endl;
		}

		// Setup min and max fields and increment (uT)
		int64_t iHmax = vmath::iround64(double(sim::Hmax) * 1.0E6);
		int64_t miHmax = -iHmax;
		int64_t parity_old;
		int64_t iH_old;
		uint64_t start_time;

		// Equilibrate system in saturation field, i.e. the largest between equilibration and maximum field set by the user
		if (sim::Heq >= sim::Hmax)
		{
			sim::H_applied = sim::Heq;
		}
		else
		{
			sim::H_applied = sim::Hmax;
		}

		// Initialise sim::integrate only if it not a checkpoint
		if (sim::load_checkpoint_flag && sim::load_checkpoint_continue_flag)
		{
		}
		else
			sim::integrate(sim::equilibration_time);
		// 这么写代码，似乎只在最开始进行了平衡，之后均未平衡

		/*
		//调试信息
		for(int i=0; i<10; ++i){
			std::cout << "[DEBUG] atom " << i
					  << " local_field_x=" << cells::local_field_array_x[i]
					  << " local_field_y=" << cells::local_field_array_y[i]
					  << " local_field_z=" << cells::local_field_array_z[i]
					  << std::endl;
		}
		//
		*/

		// Hinc must be positive
		int64_t iHinc = vmath::iround64(double(fabs(sim::Hinc)) * 1.0E6);

		int64_t Hfield;
		int64_t iparity = sim::parity;
		parity_old = iparity;

		// Save value of iH from previous simulation
		if (sim::load_checkpoint_continue_flag)
			iH_old = static_cast<int64_t>(sim::iH); //  int(sim::iH);

		// Perform Field Loop -parity
		while (iparity < 2)
		{
			// If checkpoint is loaded with continue flag, then set up correctly max,min field values
			if (sim::load_checkpoint_flag && sim::load_checkpoint_continue_flag)
			{
				// necessary to upload value of iH_old when loading the checkpoint !!!
				iH_old = static_cast<int64_t>(sim::iH);
				// Setup min and max fields and increment (uT)
				if (parity_old < 0)
				{
					if (iparity < 0)
						miHmax = iH_old;
					else if (iparity > 0 && iH_old <= 0)
						miHmax = iH_old; // miHmax=(iHmax-iHinc);
					else if (iparity > 0 && iH_old > 0)
						miHmax = -(iHmax);
				}
				else if (parity_old > 0)
					miHmax = iH_old;
				Hfield = miHmax;
			}
			else
				Hfield = miHmax;

			// Perform Field Loop -field
			while (Hfield <= iHmax)
			{

				// Set applied field (Tesla)
				sim::H_applied = double(Hfield) * double(iparity) * 1.0e-6;

				// Reset start time
				start_time = sim::time;

				// Reset mean magnetisation counters
				stats::reset();

				// Integrate system
				while (sim::time < sim::loop_time + start_time)
				{

					// Integrate system
					sim::integrate(sim::partial_time);

					// Calculate mag_m, mag
					stats::update();

				} // End of integration loop

				// Increment of iH
				Hfield += iHinc;
				sim::iH = Hfield; // sim::iH+=iHinc;

				// Output to screen and file after each field
				vout::data();

			} // End of field loop

			// Increment of parity
			iparity += 2;
			sim::parity = iparity;

		} // End of parity loop

		return EXIT_SUCCESS;

	} // End of hysteresis program

} // end of namespace program

/*伪代码
初始化磁场参数
预处理系统（积分到平衡）

for (极性方向) {
	for (磁场从最小到最大) {
		设置外加磁场
		重置统计
		while (积分时间未到) {
			积分
			更新统计
		}
		输出当前磁场下统计数据
	}
	切换极性
}
	具体实现积分的是 sim::integrate 函数
*/