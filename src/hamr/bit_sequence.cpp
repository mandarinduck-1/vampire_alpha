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
#include "errors.hpp"
#include "hamr.hpp"
#include "vio.hpp"

// hamr headers
#include "internal.hpp"

namespace hamr{
   namespace internal{

      //-----------------------------------------------------------------------------
      // Function to generate single tone sequence
      //-----------------------------------------------------------------------------
		//这个函数的作用是生成默认的交替比特序列（如 -1, 1, -1, 1...）。
	    void create_singletone_vector(){

			if(!hamr::internal::create_singletone){
				std::cout << "Bit sequence provided by user" << std::endl;
				zlog << zTs() << "Bit sequence provided by user" << std::endl;
				//zlog << zTs()自定义日志工具，zTs() 可能用于添加时间戳
				return;
			}
			//检查全局标志 create_singletone，如果为 false，表示用户提供了自定义序列，直接返回。

			// output informative message
			zlog << zTs() << "Creating singletone bit sequence ..." << std::endl;
			//输出日志信息，提示正在生成默认序列。

			hamr::internal::bit_sequence.clear();
			// clear() 清空向量，确保从空序列开始。
			for(int i=0; i<hamr::internal::num_bits; ++i){
				//生成 num_bits 个比特（num_bits 是用户定义的全局变量）。
				int bit = i%2;
				if(bit==0){ bit=-1;}
				//将 0 转换为 -1（可能是为了表示磁化的负方向）。
				hamr::internal::bit_sequence.push_back(bit);
				//将生成的比特追加到 bit_sequence 向量中。
			}

			return;
		}



      //-----------------------------------------------------------------------------
      // Function to check that user defined bit sequence is consistent with system
		// size and "number-of-bits" parameter
      //-----------------------------------------------------------------------------
		//检查比特序列是否与系统参数（轨道数、每轨道比特数）兼容。
	    void check_sequence_length(){

			zlog << zTs() << "Checking length of bit sequence ..." << std::endl;
			//日志记录，提示正在检查序列长度。

      	// Check that "number-of-bits" and size of bit sequence provided are consistent
      	if(static_cast<size_t>(hamr::internal::num_bits) > hamr::internal::bit_sequence.size()){
            std::cout << "Warning: Requested number of bits "  << hamr::internal::num_bits
                     << " larger than size of the provided bit sequence=" << hamr::internal::bit_sequence.size()
                     << ". Adjusting to " << hamr::internal::bit_sequence.size() << std::endl;
            zlog << zTs() << "Warning: Requested number-of-bit "  << hamr::internal::num_bits
                        << " larger than size of the provided bit sequence=" << hamr::internal::bit_sequence.size()
                        << ". Adjusting to " << hamr::internal::bit_sequence.size() << std::endl;
            hamr::internal::num_bits = hamr::internal::bit_sequence.size();
      	}
		//static_cast<size_t>将 num_bits（int）转换为无符号整数类型 size_t（与 vector.size() 类型匹配）。
		//如果用户请求的比特数 num_bits 大于实际序列长度，则：
        //输出警告。
        //将 num_bits 调整为实际序列长度。
      	else if(static_cast<size_t>(hamr::internal::num_bits) < hamr::internal::bit_sequence.size()){
				std::cout << "Warning: requested number of bits "  << hamr::internal::num_bits
				         << " smaller than size of provided bit sequence=" << hamr::internal::bit_sequence.size()
				         << ". Trimming bit sequence." << std::endl;
				zlog << zTs() << "Warning: number of bits "  << hamr::internal::num_bits
				            << " smaller than size of provided bit sequence=" << hamr::internal::bit_sequence.size()
				            << ". Trimming bit sequence." << std::endl;
				hamr::internal::bit_sequence.resize(hamr::internal::num_bits);
			}
			//如果序列比 num_bits 长，截断多余部分。

      	// Check that number of bit requested is compatible with system size
			if(hamr::internal::num_bits > hamr::internal::num_tracks * hamr::internal::bits_per_track){
				const int num_bits_total = hamr::internal::num_tracks * hamr::internal::bits_per_track;
				std::cout << "Warning: requested number of bits "  << hamr::internal::num_bits
				         << " too big for system size. Reducing to " << num_bits_total << std::endl;
				zlog << zTs() << "Warning: number of bits "  << hamr::internal::num_bits
				               << " too big for system size. Reducing to " << num_bits_total << std::endl;
				hamr::internal::num_bits = num_bits_total;
				hamr::internal::bit_sequence.resize(num_bits_total);
			}
			//num_tracks * bits_per_track 表示硬盘总比特容量。
            //如果请求的比特数超出容量：
            //调整 num_bits 并截断序列。

			else if(hamr::internal::num_bits < hamr::internal::num_tracks * hamr::internal::bits_per_track){
				// Determine how many zero padding bits are required
				int num_padding = hamr::internal::num_tracks * hamr::internal::bits_per_track - hamr::internal::num_bits;
				for(int i=0; i<num_padding; ++i){
					hamr::internal::bit_sequence.push_back(0);
				}
				std::cout << "Warning: provided bit sequence too short. Adding " << num_padding << " \'0\' bits to reach end of track" << std::endl;
				zlog << zTs() << "Warning: provided bit sequence too short. Adding " << num_padding << " \'0\' bits to reach end of track" << std::endl;
			}
			//补零填充如果序列太短，用 0 填充至总容量（0 可能表示“无写入”）。

			return;
		}


   } // end of namespace internal
} // end of namespace hamr
