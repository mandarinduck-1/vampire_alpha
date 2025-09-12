//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) 2025 Tang Yuhan. All rights reserved.
//
//------------------------------------------------------------------------------
//

// Standard Libraries
#include <vector>
#include <cstdint>

// Vampire Header files
#include "vmath.hpp"
#include "errors.hpp"
#include "sim.hpp"
#include "stats.hpp"
#include "vio.hpp"

// 这是唐愈涵写入的，结构体定义在program/internal.hpp
#include "internal.hpp"

namespace program
{

   /// @brief Function to calculate the segmented hysteresis loop
   ///
   /// @details sim:program=segmented-hysteresis-loop
   ///          支持多个磁场扫描段，每段的最小/最大磁场、步进、平衡步数和积分步数可独立设置。
   ///
   /// @param segments 每个段的参数列表
   ///

   void segmented_hysteresis_loop()
   {

      const auto &segments = program::internal::segmented_hyst_segments;
      if (err::check == true)
      {
         std::cout << "program::segmented-hysteresis-loop has been called" << std::endl;
      }

      for (size_t i = 0; i < segments.size(); ++i)
      {
         const auto &seg = segments[i];

         // 用整数微特斯拉循环，保证精度
         int64_t iHmin = vmath::iround64(seg.Hmin * 1.0E6);
         int64_t iHmax = vmath::iround64(seg.Hmax * 1.0E6);
         int64_t iHinc = vmath::iround64(seg.Hinc * 1.0E6);

         // Equilibrate system in segment start field
         sim::H_applied = seg.Hmin;

         // 允许min < max，步进可正可负，这是关键的判断方法
         for (int64_t iH = (iHinc > 0) ? iHmin : iHmax;    // 初始值由方向决定
              (iHinc > 0) ? (iH <= iHmax) : (iH >= iHmin); // 终止条件由方向决定
              iH += iHinc)
         {
            sim::H_applied = static_cast<double>(iH) * 1.0E-6; // 转回特斯拉

            /*std::cout << "[调试] 段 " << i + 1
                      << " 平衡步数(equil_steps): " << seg.equil_steps
                      << " 积分步数(loop_steps): " << seg.loop_steps << std::endl;*/

            sim::integrate(seg.equil_steps);

            uint64_t start_time = sim::time;
            stats::reset();

            sim::integrate(seg.loop_steps);
            stats::update();

            vout::data();
         }
      }

      return;
   }

} // end of namespace program