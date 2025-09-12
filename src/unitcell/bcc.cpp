//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Richard F L Evans 2016. All rights reserved.
//
//   Email: richard.evans@york.ac.uk
//
//------------------------------------------------------------------------------
//

// C++ standard library headers
#include <cmath>

// Vampire headers
#include "unitcell.hpp"

// unitcell module headers
#include "internal.hpp"

namespace unitcell{
namespace internal{

void build_body_centred_cubic(unitcell::unit_cell_t& unit_cell){

   // Set basic unit cell properties
   //设置单胞尺寸（立方结构，三个轴长度相等）
   unit_cell.dimensions[0] = 1.0;  // a轴
   unit_cell.dimensions[1] = 1.0;  // b轴
   unit_cell.dimensions[2] = 1.0;  // c轴

   // 设置形状矩阵（正交单位矩阵 - 立方结构）
   unit_cell.shape[0][0]=1.0;   // x轴方向
   unit_cell.shape[0][1]=0.0;   
   unit_cell.shape[0][2]=0.0;  

   unit_cell.shape[1][0]=0.0;
   unit_cell.shape[1][1]=1.0;   // y轴方向
   unit_cell.shape[1][2]=0.0;

   unit_cell.shape[2][0]=0.0;
   unit_cell.shape[2][1]=0.0;
   unit_cell.shape[2][2]=1.0;   // z轴方向

   //unit_cell.shape = [1.0  0.0  0.0]
   //                  [0.0  1.0  0.0]
   //                  [0.0  0.0  1.0]
   //上面构建了一个正交晶系的几何变换矩阵
   //计算方法为
   //实际坐标 = shape_matrix × 分数坐标 × dimensions
   //[X]   [1.0  0.0  0.0]   [x]     [a]
   //[Y] = [0.0  1.0  0.0] × [y]  ×  [b]
   //[Z]   [0.0  0.0  1.0]   [z]     [c]

   unit_cell.lcsize=2;  //单胞中有两个原子
   unit_cell.hcsize=2;  //2种配位环境
   unit_cell.interaction_range=1;  //相互作用范围
   unit_cell.atom.resize(2);  //调整原子数组大小为2
   unit_cell.surface_threshold=8;  //表面判断阈值（8个近邻）
   //-----------------------------
   unit_cell.atom[0].x=0.0;
   unit_cell.atom[0].y=0.0;
   unit_cell.atom[0].z=0.0;  
   //定义原子0的位置在角顶（0，0，0）处
   unit_cell.atom[0].mat = uc::internal::sublattice_materials ? 0 : 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[0].lc=0;  //局部环境ID
   unit_cell.atom[0].hc=0;  //配位环境ID
   unit_cell.atom[0].ni=8;  //8个最近邻
   //-----------------------------
   unit_cell.atom[1].x=0.5;
   unit_cell.atom[1].y=0.5;
   unit_cell.atom[1].z=0.5;
   //定义原子1的位置在体心（0.5，0.5，0.5）处
   unit_cell.atom[1].mat = uc::internal::sublattice_materials ? 1 : 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[1].lc=1;  //局部环境ID
   unit_cell.atom[1].hc=1;  //配位环境ID
   unit_cell.atom[1].ni=8;  //8个最近邻
   //unit_cell.atom[0].mat = uc::internal::sublattice_materials ? 0 : 0;
   //unit_cell.atom[1].mat = uc::internal::sublattice_materials ? 1 : 0;
   //用于开启亚晶格结构，如果sublattice_materials = true：角顶原子为材料0，体心原子为材料1
   //如果sublattice_materials = false：两种原子都是材料0
   //-----------------------------

   unit_cell.cutoff_radius = sqrt(3.0)/2.0; // normalised to unit cell size
   // 设置最近邻的截止半径为 sqrt(3)/2 ≈ 0.866
   // 设置这个值是因为最近邻距离是体对角线的一半，从而确保只有最近邻的相互作用

   uc::internal::calculate_interactions(unit_cell);
   // 计算相互作用

   // Set actual unit cell size after calculating interactions
   unit_cell.dimensions[0] *= unitcell::internal::unit_cell_size_x;
   unit_cell.dimensions[1] *= unitcell::internal::unit_cell_size_y;
   unit_cell.dimensions[2] *= unitcell::internal::unit_cell_size_z;
   // 将归一化的单胞尺寸转化为实际物理尺寸

   return;

}

} // end of internal namespace
} // end of unitcell namespace
