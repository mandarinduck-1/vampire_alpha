//-----------------------------------------------------------------------------
//
// This header file is part of the VAMPIRE open source package under the
// GNU GPL (version 2) licence (see licence file for details).
//
// (c) R F L Evans 2014. All rights reserved.
//
//-----------------------------------------------------------------------------
//
//   Functions to calculate dynamic lateral and vertical temperature
//   profiles with the two temperature model. Laser has gaussian absorption
//   profile in x,y and an exponential decrease in absorption with increasing
//   depth (z).
//
//                               |
//                               |
//                               |
//                               |
//                               |
//                              _-_
//                             /   \
//                         __--     --__
//                     ----------------------
//                     |  |  |  |  |  |  |  |
//                     ----------------------
//                     |  |  |  |  |  |  |  |
//                     ----------------------
//
//   The deposited laser energy is modified laterally by a gaussian heat profile
//   given by
//
//   P(r) = exp (-4 ln(2.0) (r**2) / (fwhm**2) )
//   这里是说作者认为激光在横向上是高斯分布
//
//   and/or vertically considering an exponential depth dependence of the laser
//   energy given by
//
//   P(z) = exp(-z/penetration-depth)
//   这里是说作者认为激光在纵向上是指数衰减的
//
//   Both of these can be combined to give a full 3D solution of the two
//   temperature model, including dynamic heat distribution within the sample.

// System headers
#include <string>
#include <vector>

// Program headers

#ifndef LOCALTEMPERATURE_H_
#define LOCALTEMPERATURE_H_

//--------------------------------------------------------------------------------
// Namespace for variables and functions to calculate localised temperature pulse
//--------------------------------------------------------------------------------
//这个部分主要是想考虑局域激光脉冲（激光加热）对自旋动力学的影响
namespace ltmp{

   //--------------------------------------------------------------------
   // Class to contain parameters for tabulated absorption profile
   //
   // Tabulated values are read from a file and added point-wise to
   // the class. During initialisation interpolating functions
   // are determined to calculate A(z)
   //
   class abs_t{

      private:

         int z_max; // maximum array value in tabulated function
         //最大深度
         double A_max; // value of absorption at z_max (used for all z > z_max)
         //对应的吸收率
         bool profile_loaded; // flag indicating profile has been loaded from file
         //标志位，表示是否已经加载了吸收率文件

         std::vector<int> z; // input z-height from surface values
         std::vector<double> A; // input absorption values
         //输入的z高度和对应的吸收率
         std::vector<double> m; // calculated m value
         std::vector<double> c; // calculated c value
         //用于插值的m和c值

      public:
         abs_t();
         bool is_set();
         void add_point(double height, double absorption);
         //添加一个点到吸收率表中
         void set_interpolation_table();
         //设置插值表
         double get_absorption_constant(double height);
         //获取指定深度的吸收率
         void output_interpolated_function(int height);
         //输出插值函数

   };

   //-----------------------------------------------------------------------------
   // Variables used for the localised temperature pulse calculation
   //-----------------------------------------------------------------------------
   extern abs_t absorption_profile; // class variable containing tabulated absorption profile
   //全局吸收剖面对象，用于存储和查询吸收率

   //-----------------------------------------------------------------------------
   // Function to check local temperature pulse is enabled
   //-----------------------------------------------------------------------------
   bool is_enabled();
   //检查局域温度脉冲是否启用

   //-----------------------------------------------------------------------------
   // Function to initialise localised temperature pulse calculation
   //-----------------------------------------------------------------------------
   void initialise(const double system_dimensions_x,
                  const double system_dimensions_y,
                  const double system_dimensions_z,
                  const std::vector<double>& atom_coords_x,
                  const std::vector<double>& atom_coords_y,
                  const std::vector<double>& atom_coords_z,
                  const std::vector<int>& atom_type_array,
                  const int num_local_atoms,
                  const double starting_temperature,
                  const double pump_power,
                  const double pump_time,
                  const double TTG,
                  const double TTCe,
                  const double TTCl,
                  const double dt,
                  const double Tmin,
                  const double Tmax);
                  //这里初始化了一些激光参数

   //-----------------------------------------------------------------------------
   // Function to copy localised thermal fields to external field array
   //-----------------------------------------------------------------------------
   void get_localised_thermal_fields(std::vector<double>& x_total_external_field_array,
                               std::vector<double>& y_total_external_field_array,
                               std::vector<double>& z_total_external_field_array,
                               const int start_index,
                               const int end_index);

   //-----------------------------------------------------------------------------
   // Function for updating localised temperature
   //-----------------------------------------------------------------------------
   void update_localised_temperature(const double start_from_start);

   //-----------------------------------------------------------------------------
   // Function to process input file parameters for ltmp settings
   //-----------------------------------------------------------------------------
   bool match_input_parameter(std::string const key, std::string const word, std::string const value, std::string const unit, int const line);

} // end of ltmp namespace

#endif // LOCALTEMPERATURE_H_
