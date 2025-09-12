//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Sarah Jenkins and Richard F L Evans 2016. All rights reserved.
//
//   Email: sj681@york.ac.uk
//
//------------------------------------------------------------------------------
//

#ifndef MICROMAGNETIC_H_
#define MICROMAGNETIC_H_

// C++ standard library headers
#include <string>

// Vampire headers
#include "micromagnetic.hpp"
#include "material.hpp"
//--------------------------------------------------------------------------------
// Namespace for variables and functions for micromagnetic module
//--------------------------------------------------------------------------------
namespace micromagnetic{

    //bool to decide whether the simulation is micromagnetic or atomsitic
    // 0 - atomistic
    // 1 - micromagnetic
    // 2 - multiscale
    // 0-原子级，1-微磁，2-多尺度
   extern int discretisation_type;

   extern bool enable_resistance;
   //是否启用电阻计算
   //initialises the lists of atomstic/micromagnetic atoms for multiscale simulations
   extern std::vector <int> list_of_atomistic_atoms;
   //原子级原子索引列表
   extern std::vector <int> list_of_none_atomistic_atoms;
   //非原子级原子索引列表
   extern std::vector <int> list_of_micromagnetic_cells;
   //微磁单元格索引列表
   extern std::vector <int> list_of_empty_micromagnetic_cells;
   //空的微磁单元格索引列表
   extern std::vector <double> atomistic_bias_field_x;
   extern std::vector <double> atomistic_bias_field_y;
   extern std::vector <double> atomistic_bias_field_z;
   //原子级偏置场向量
   //variables to store the numbers of atomistic/ microamgnetic atoms for multiscale simulations
   extern int number_of_atomistic_atoms;
   //原子级原子数
   extern int number_of_none_atomistic_atoms;
   //非原子级原子数
   extern int number_of_micromagnetic_cells;
   //微磁单元格数

   extern double MR_resistance;
   //巨磁电阻值

   //vector to store whether cells are micromagnetic or atomistic
   extern std::vector < bool > cell_discretisation_micromagnetic;
   //标记每个单元格是微磁还是原子级

   //set the integrator for microamgnetic simulations
   //0 - LLG
   //1 - LLB
   extern int integrator;
   //选择积分器类型

   //number of micromagnetic steps per atomistic step
   extern int num_atomic_steps_mm;
   //每个原子级步骤的微磁步骤数

   //bool to enable the output of a boltzman distribution
   extern bool boltzman;
   //是否输出玻尔兹曼分布

   //varibles for the Boltzman distribution
   extern double mean_M;
   //平均磁化和计数器
   extern double counter;
   extern std::vector < std::vector < double > > P;
   extern std::vector < double > P1D;

   void outputs();

   //--------------------------------------------------------------------
   //     Function declorations
   //--------------------------------------------------------------------

   //atomsitic LLG
   int atomistic_LLG_Heun();
   //原子级LLG-Heun积分器

   //function to initialise the atomistic LLG
   int atomistic_LLGinit();
   //原子级LLG初始化

   //micromagnetic LLB
   int LLB( std::vector <int>& local_cell_array,
            int num_steps,
            int num_cells,
            int num_local_cells,
            double temperature,
            std::vector<double>& x_mag_array,
            std::vector<double>& y_mag_array,
            std::vector<double>& z_mag_array,
            double Hx,
            double Hy,
            double Hz,
            double H,
            double dt,
            std::vector <double>& volume_array);
            //微磁LLB动力学

    //micromagnetic LLG
    int LLG( std::vector <int> &local_cell_array,
             int num_steps,
             int num_cells,
             int num_local_cells,
             double temperature,
             std::vector<double>& x_mag_array,
             std::vector<double>& y_mag_array,
             std::vector<double>& z_mag_array,
             double Hx,
             double Hy,
             double Hz,
             double H,
             double dt,
             std::vector <double> &volume_array);
             //微磁LLG动力学

   //-----------------------------------------------------------------------------
   // Function to initialise micromagnetic module
   //-----------------------------------------------------------------------------
   void initialize( int num_local_cells,
                    int num_cells,
                    int num_atoms,
                    int num_materials,
                    std::vector<int> cell_array,
                    std::vector<int> neighbour_list_array,
                    std::vector<int> neighbour_list_start_index,
                    std::vector<int> neighbour_list_end_index,
                    std::vector<int> type_array,
                    std::vector <mp::materials_t> material,
                    std::vector <double> x_coord_array,
                    std::vector <double> y_coord_array,
                    std::vector <double> z_coord_array,
                    std::vector <double> volume_array,
                    double Temperature,
                    double num_atoms_in_unit_cell,
                    double system_dimensions_x,
                    double system_dimensions_y,
                    double system_dimensions_z,
                    std::vector<int> local_cell_array);



   //---------------------------------------------------------------------------
   // Function to process input file parameters for micromagnetic module
   //---------------------------------------------------------------------------
   bool match_input_parameter(std::string const key, std::string const word, std::string const value, std::string const unit, int const line);

   //---------------------------------------------------------------------------
   // Function to process material parameters
   //---------------------------------------------------------------------------
   bool match_material_parameter(std::string const word, std::string const value, std::string const unit, int const line, int const super_index, const int sub_index);

} // end of micromagnetic namespace

#endif //MICROMAGNETIC_H_
