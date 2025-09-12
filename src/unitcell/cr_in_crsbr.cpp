//------------------------------------------------------------------------------
//
//   This file was implemented in the VAMPIRE software package specifically 
//   for modeling the crystal lattice of CrSBr-like compounds.Here I define 
//   only Cr in the lattice for simplicity.
//
//   (c) Tang Yuhan 2025. All rights reserved.
//
//   Email: tangyh22@mails.tsinghua.edu.cn
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

void build_cr_in_crsbr(unitcell::unit_cell_t& unit_cell){

   double a,b,c;
   a = 3.508;
   b = 4.763;
   c = 7.959;
   // 基本单胞设置 - 变形体心立方
   unit_cell.dimensions[0] = 1.0;  // a轴 = 3.508 Å
   unit_cell.dimensions[1] = b/a;  // b轴 = 4.763 Å  
   unit_cell.dimensions[2] = c/a;  // c轴 = 7.959 Å
   // 变形单胞尺寸 = (1.0, 1.358, 2.269)
   //y和z轴的设置参考了bcc110.ccp和fcc111.cpp的设置方法
   //这个尺寸只对生成分数坐标的时候计算阶段半径有用
   //在输入文件里面只应该设置x轴的长度尺寸，另外两个尺寸不应该再输入，否则会重复计算
   //这么做之后，三个轴就相当于是一样长了，方便之后的计算

   // 正交晶系形状矩阵
   unit_cell.shape[0][0]=1.0; unit_cell.shape[0][1]=0.0; unit_cell.shape[0][2]=0.0;
   unit_cell.shape[1][0]=0.0; unit_cell.shape[1][1]=1.0; unit_cell.shape[1][2]=0.0;
   unit_cell.shape[2][0]=0.0; unit_cell.shape[2][1]=0.0; unit_cell.shape[2][2]=1.0;

   // 简化为只有Cr原子的磁性模型
   unit_cell.lcsize = 2;              // 只有2个Cr原子
   unit_cell.hcsize = 1;              // 只有1种材料类型
   unit_cell.interaction_range = 1;    
   unit_cell.atom.resize(2);        
   unit_cell.surface_threshold = 6;   

   //-----------------------------
   // 定义变形体心立方中的Cr原子位置
   //-----------------------------
   
   // Cr1原子 - 设为原点（等效于体心立方的一个顶点）
   unit_cell.atom[0].x = 0.0;
   unit_cell.atom[0].y = 0.0;
   unit_cell.atom[0].z = 0.0;
   unit_cell.atom[0].mat = 0;
   unit_cell.atom[0].lc = 0;
   unit_cell.atom[0].hc = 0;
   unit_cell.atom[0].ni = 8;          // 体心立方中每个原子有8个最近邻
   // 关于配位数的设置我也不确定，但是fcc111.cpp也设置了近邻数，我不知道这个值有什么意义？

   // Cr2原子 - 偏心的"体心"位置
   unit_cell.atom[1].x = 0.5;         // 体心的x坐标
   unit_cell.atom[1].y = 0.5;         // 体心的y坐标
   unit_cell.atom[1].z = 0.127;       // 体心偏移：但这个之我也不确定是不是准确的，没找到直接的文献
   unit_cell.atom[1].mat = 0;
   unit_cell.atom[1].lc = 1;
   unit_cell.atom[1].hc = 0;
   unit_cell.atom[1].ni = 8;

   //-----------------------------
   // 计算三种Cr-Cr交换距离
   //-----------------------------
   
   // 已知：a = 3.508 Å, b = 4.763 Å, c = 7.959 Å
      // J₁ (a轴方向): 距离 = a = 3.508 Å 相对距离为1
   // J₂ (b轴方向): 距离 = b = 4.763 Å  相对距离为 4.763 / 3.508 = 1.358
   // J₃ (对角线):  距离 = √[(0.5-0.0)² + (0.5×1.358-0.0)² + (0.127×2.269-0.0)²]
   // = √[0.5² + (0.679)² + (0.288)²]
   //  = √[0.25 + 0.461 + 0.083]
   //  = √0.794 = 0.891
   // 设置截断半径包含所有三种交换（最大距离是b轴）
   // 在smfe12n.cpp的注释里写明了是对第一个轴的长度做归一化的
   unit_cell.cutoff_radius = 2.3;

   // 计算相互作用
   uc::internal::calculate_interactions(unit_cell);

   // 设置实际单胞尺寸
   unit_cell.dimensions[0] *= unitcell::internal::unit_cell_size_x;  // 3.508 Å
   unit_cell.dimensions[1] *= unitcell::internal::unit_cell_size_y;  // 
   unit_cell.dimensions[2] *= unitcell::internal::unit_cell_size_z;  //

   return;
}
}
}