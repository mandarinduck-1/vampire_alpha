//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Richard F L Evans 2022. All rights reserved.
//
//   Email: richard.evans@york.ac.uk
//
//------------------------------------------------------------------------------
//

// C++ standard library headers

// module headers
#include "internal.hpp"

//------------------------------------------------------------------------------
// Test to verify the correct exchange energy for different configurations
//------------------------------------------------------------------------------
bool exchange_test(std::string dir, double result, std::string executable){

   // get root directory
   std::string path = std::filesystem::current_path();

   // fixed-width output for prettiness
   std::stringstream test_name;
   test_name << "Testing exchange energy for " << dir;
   std::cout << std::setw(60) << std::left << test_name.str() << " : " << std::flush;

   // change directory
   if( !vt::chdir(path+"/data/"+dir) ) return false;

   // run vampire
   int vmp = vt::system(executable);
   if( vmp != 0){
      std::cerr << "Error running vampire. Returning as failed test." << std::endl;
      return false;
   }

   std::string line;

   // open output file
   std::ifstream ifile;
   ifile.open("output");

   // read value after header
   for(int i=0; i<9; i++) getline(ifile, line);
   std::stringstream liness(line);
   double value = 0.0;
   liness >> value;

   // cleanup
   vt::system("rm output log");

   // return to parent directory
   if( !vt::chdir(path) ) return false;

   // now test value obtained from code
   const double ratio = value/result;

   // return based on test result
   if(ratio >0.99999 && ratio < 1.00001){
      std::cout << "OK" << std::endl;
      return true;
   }
   else{
      std::cout << "FAIL | expected: " << result << "\tobtained:  " << value << "\t" << line << std::endl;
      return false;
   }

}
