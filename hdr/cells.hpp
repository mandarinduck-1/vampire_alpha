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

#ifndef CELLS_H_
#define CELLS_H_

// C++ standard library headers
#include <string>
#include <vector>

// Vampire headers

//--------------------------------------------------------------------------------
// Namespace for variables and functions for cells module
//--------------------------------------------------------------------------------
namespace cells{

   extern int num_atoms_in_unit_cell;
   // 单位晶胞中的原子数
   extern int num_cells; /// number of macro-cells
   //宏胞（cell）的总数
   extern int num_local_cells; /// number of macro-cells
   //本地宏胞数（用于并行计算）

   extern double macro_cell_size; /// lateral size of local macro-cells (A)
   extern double macro_cell_size_x; /// lateral size of local macro-cells (A)
   extern double macro_cell_size_y; /// lateral size of local macro-cells (A)
   extern double macro_cell_size_z; /// lateral size of local macro-cells (A)
   //宏胞的尺寸

   extern std::vector <int> local_cell_array;
   //本地宏胞索引
   extern std::vector <int> num_atoms_in_cell; /// number of atoms in each cell
   //每个宏胞中的原子数
   extern std::vector <int> num_atoms_in_cell_global; /// global number of atoms in each cell
   //全局中每个宏胞的原子数
   extern std::vector < std::vector <int> > index_atoms_array;
   //每个宏胞中原子的索引列表
   extern std::vector<int> index_atoms_array1D;
   //一维原子索引列表

   extern std::vector<double> volume_array;
   //每个宏胞的体积
   extern std::vector<double> cell_coords_array_x; /// arrays to store cells positions
   extern std::vector<double> cell_coords_array_y;
   extern std::vector<double> cell_coords_array_z;
   //宏胞的坐标
   extern std::vector < std::vector <double> > atom_in_cell_coords_array_x;
   extern std::vector < std::vector <double> > atom_in_cell_coords_array_y;
   extern std::vector < std::vector <double> > atom_in_cell_coords_array_z;
   //宏包内的原子坐标

   extern std::vector<int> cell_id_array;
   //宏包编号
   extern std::vector<int> atom_cell_id_array;
   //每个原子所属的宏胞编号
   extern std::vector<double> mag_array_x; /// arrays to store cells magnetisation
   extern std::vector<double> mag_array_y;
   extern std::vector<double> mag_array_z;
   //宏胞的磁化矢量
   extern std::vector<double> field_array_x; /// arrays to store cells field
   extern std::vector<double> field_array_y;
   extern std::vector<double> field_array_z;
   //宏胞的磁场分量

   extern std::vector<double> pos_and_mom_array;
   extern std::vector<double> pos_array;
   //用于存储原子的坐标和自旋

   extern std::vector < double > num_macro_cells_fft; /// lateral size of local macro-cells (A)
   extern std::vector<double> fft_cell_id_array;
   //用于FFT计算的宏胞数和宏胞编号

   //---------------------------------------------------------------------------
   // Function to calculate magnetisation in cells
   //---------------------------------------------------------------------------
   extern int mag();

   //-----------------------------------------------------------------------------
   // Function to initialise cells module
   //-----------------------------------------------------------------------------
   void initialize(const double system_dimensions_x,
                   const double system_dimensions_y,
                   const double system_dimensions_z,
                   const double unit_cell_size_x,
                   const double unit_cell_size_y,
                   const double unit_cell_size_z,
                   const std::vector<double>& atom_coords_x,
                   const std::vector<double>& atom_coords_y,
                   const std::vector<double>& atom_coords_z,
                   const std::vector<int>& atom_type_array,
                   const std::vector<int>& atom_cell_id_array,
                   const int num_total_atoms_for_dipole,
                   const int num_atoms
   );



   //局部场实现
   extern std::vector<double> local_field_array_x;
   extern std::vector<double> local_field_array_y;
   extern std::vector<double> local_field_array_z;
   struct LocalFieldRegion {
        int material_type;
        double x_min, x_max;
        double y_min, y_max;
        double z_min, z_max;
        double field_x, field_y, field_z;

        // 默认构造函数，初始化所有成员
        LocalFieldRegion()
            : material_type(0),
              x_min(0), x_max(0),
              y_min(0), y_max(0),
              z_min(0), z_max(0),
              field_x(0), field_y(0), field_z(0) {}
    };
   extern int g_num_local_field_regions;
   extern std::vector<LocalFieldRegion> g_local_field_regions;
   std::vector<LocalFieldRegion> read_local_field_regions_from_input();
   // 局部场应用函数声明
    void apply_local_field(
    const std::vector<LocalFieldRegion>& regions,
    const std::vector<int>& atom_type_array,
    const std::vector<double>& atom_coords_x,
    const std::vector<double>& atom_coords_y,
    const std::vector<double>& atom_coords_z,
    const std::vector<int>& atom_cell_id_array,
    int num_atoms
   );//这一部分是唐愈涵加的，希望可以实现局部场


   //---------------------------------------------------------------------------
   // Function to process input file parameters for cells module
   //---------------------------------------------------------------------------
   bool match_input_parameter(std::string const key, std::string const word, std::string const value, std::string const unit, int const line);
   //处理输入文件中的参数

   //---------------------------------------------------------------------------
   // Function to process material parameters
   //---------------------------------------------------------------------------
   bool match_material_parameter(std::string const word, std::string const value, std::string const unit, int const line, int const super_index, const int sub_index);
   //处理材料文件中的参数

} // end of cells namespace

#endif //CELLS_H_
