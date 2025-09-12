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

void build_cr_in_crbr3(unitcell::unit_cell_t& unit_cell){

   // rescale y and z unit cell size to preserve unit cell dimensions
   unit_cell.dimensions[0]=1.0*sqrt(3.0);
   unit_cell.dimensions[1]=1.0/3.0*sqrt(3.0)*sqrt(3.0);
   unit_cell.dimensions[2]=2.907;
   //已经充分确定了晶格结构，a和b轴是不用动的，唯一要确定的是c和a轴的比值
   //取a=b=6.260A，c=18.2A，c/a=2.907
   //但是在输入文件里面应当输入6.26，因为晶格常数不是定义为六边形的边长
   //而是定义为六边形的一个弦长，可以参考石墨烯的布拉伐格子定义

   unit_cell.shape[0][0]=1.0;
   unit_cell.shape[0][1]=0.0;
   unit_cell.shape[0][2]=0.0;

   unit_cell.shape[1][0]=0.0;
   unit_cell.shape[1][1]=1.0;
   unit_cell.shape[1][2]=0.0;

   unit_cell.shape[2][0]=0.0;
   unit_cell.shape[2][1]=0.0;
   unit_cell.shape[2][2]=1.0;

   unit_cell.lcsize=2;
   unit_cell.hcsize=3;
   unit_cell.interaction_range=1;
   unit_cell.atom.resize(12);
   unit_cell.surface_threshold=3;
   //-----------------------------
   unit_cell.atom[0].x = 0.0;
   unit_cell.atom[0].y = 0.5;
   unit_cell.atom[0].z = 0.0;
   unit_cell.atom[0].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[0].lc = 0;
   unit_cell.atom[0].hc = 0;
   unit_cell.atom[0].ni = 3;
   //-----------------------------
   unit_cell.atom[1].x = 1.0/6.0;
   unit_cell.atom[1].y = 0.0;
   unit_cell.atom[1].z = 0.0;
   unit_cell.atom[1].mat =  0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[1].lc = 0;
   unit_cell.atom[1].hc = 0;
   unit_cell.atom[1].ni = 3;
   //-----------------------------
   unit_cell.atom[2].x = 0.5;
   unit_cell.atom[2].y = 0.0;
   unit_cell.atom[2].z = 0.0;
   unit_cell.atom[2].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[2].lc = 0;
   unit_cell.atom[2].hc = 0;
   unit_cell.atom[2].ni = 3;
   //-----------------------------
   unit_cell.atom[3].x = 2.0/3.0;
   unit_cell.atom[3].y = 0.5;
   unit_cell.atom[3].z = 0.0;
   unit_cell.atom[3].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[3].lc = 0;
   unit_cell.atom[3].hc = 0;
   unit_cell.atom[3].ni = 3;
   //-----------------------------
   unit_cell.atom[4].x = 1.0/6.0;
   unit_cell.atom[4].y = 0.0;
   unit_cell.atom[4].z = 1.0/3.0;
   unit_cell.atom[4].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[4].lc = 0;
   unit_cell.atom[4].hc = 1;
   unit_cell.atom[4].ni = 3;
   //-----------------------------
   unit_cell.atom[5].x = 1.0/3.0;
   unit_cell.atom[5].y = 0.5;
   unit_cell.atom[5].z = 1.0/3.0;
   unit_cell.atom[5].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[5].lc = 0;
   unit_cell.atom[5].hc = 1;
   unit_cell.atom[5].ni = 3;
   //-----------------------------
   unit_cell.atom[6].x = 2.0/3.0;
   unit_cell.atom[6].y = 0.5;
   unit_cell.atom[6].z = 1.0/3.0;
   unit_cell.atom[6].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[6].lc = 0;
   unit_cell.atom[6].hc = 1;
   unit_cell.atom[6].ni = 3;
   //-----------------------------
   unit_cell.atom[7].x = 5.0/6.0;
   unit_cell.atom[7].y = 0.0;
   unit_cell.atom[7].z = 1.0/3.0;
   unit_cell.atom[7].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[7].lc = 0;
   unit_cell.atom[7].hc = 1;
   unit_cell.atom[7].ni = 3;
   //-----------------------------
   unit_cell.atom[8].x = 0.0;
   unit_cell.atom[8].y = 0.5;
   unit_cell.atom[8].z = 2.0/3.0;
   unit_cell.atom[8].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[8].lc = 0;
   unit_cell.atom[8].hc = 2;
   unit_cell.atom[8].ni = 3;
   //-----------------------------
   unit_cell.atom[9].x = 1.0/3.0;
   unit_cell.atom[9].y = 0.5;
   unit_cell.atom[9].z = 2.0/3.0;
   unit_cell.atom[9].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[9].lc = 0;
   unit_cell.atom[9].hc = 2;
   unit_cell.atom[9].ni = 3;
   //-----------------------------
   unit_cell.atom[10].x = 0.5;
   unit_cell.atom[10].y = 0.0;
   unit_cell.atom[10].z = 2.0/3.0;
   unit_cell.atom[10].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[10].lc = 0;
   unit_cell.atom[10].hc = 2;
   unit_cell.atom[10].ni = 3;
   //-----------------------------
   unit_cell.atom[11].x = 5.0/6.0;
   unit_cell.atom[11].y = 0.0;
   unit_cell.atom[11].z = 2.0/3.0;
   unit_cell.atom[11].mat = 0; // if sublattice material is defined, then identify at as same as ID
   unit_cell.atom[11].lc = 0;
   unit_cell.atom[11].hc = 2;
   unit_cell.atom[11].ni = 3;

   unit_cell.cutoff_radius = 1.2; // normalised to unit cell size
   //第五近邻是7.2283，a=6.26，但是实际横轴长为a*sqrt(3.0)=10.843
   //7.2283/a = 1.15,7.2283/(a*sqrt(3.0)) = 0.667

   uc::internal::calculate_interactions(unit_cell);

   // Set actual unit cell size after calculating interactions
   unit_cell.dimensions[0] *= unitcell::internal::unit_cell_size_x;
   unit_cell.dimensions[1] *= unitcell::internal::unit_cell_size_y;
   unit_cell.dimensions[2] *= unitcell::internal::unit_cell_size_z;

   return;

}

} // end of internal namespace
} // end of unitcell namespace
