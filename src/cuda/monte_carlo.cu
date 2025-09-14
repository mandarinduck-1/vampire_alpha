
#include "atoms.hpp"
#include "exchange.hpp"
#include "vio.hpp"
#include "cuda_utils.hpp"
#include "internal.hpp"
#include "data.hpp"
#include "material.hpp"
#include "random.hpp"
#include "cuda.hpp"
#include "cells.hpp" // 添加 cells 头文件

#include "spin_fields.hpp"

#include "montecarlo.hpp"
#include "monte_carlo.hpp"

#include <thrust/device_vector.h>

#include "../src/montecarlo/internal.hpp"

#include <vector>

namespace vcuda
{

    //--------------------------------------------------------------------------
    // Function to perform a single Monte Carlo step
    //--------------------------------------------------------------------------
    void mc_step()
    {

#ifdef CUDA
        // check for cuda initialization, and initialize if necessary
        if (!internal::mc::initialised)
            internal::mc::initialise();
        // perform a single monte carlo step
        internal::mc::__mc_step();

#endif

        return;
    }

    namespace internal
    {
        namespace mc
        {

            bool initialised(false);

            // Number of sublattices
            int M;

            cu_real_t step_size;

            // Array to store the colour of each atom
            std::vector<int> atom_colour;

            // Create a set of lists giving which atoms have which colours
            std::vector<std::vector<int>> colour_list;

            // Index starting point in continuous array
            std::vector<int> sl_start;

            // device array for atoms in (sub lattice)
            int *d_sl_atoms;

            int *d_accepted;

            int64_t seed;
            curandGenerator_t gen;

            // Arrays for random spin vector and acceptance probability
            cu_real_t *d_rand_spin;
            cu_real_t *d_rand_accept;

            int colour_split()
            {

                std::cout << "Starting graph colouring" << std::endl;

                int N = ::atoms::num_atoms;

                // Array to store the colour of each atom
                // All are initially uncoloured (=-1)
                std::vector<int> atom_colour;
                atom_colour.assign(N, -1);

                // initial number of colours
                M = 2;

                int Ncoloured = 0;

                // Starting atom
                int iatom = 0;

                // Array to store the number of each colour that nbr the atoms
                std::vector<int> nbr_colours(M);

                do
                {

                    // std::cout << Ncoloured << "  " << iatom << "  " << atom_colour[iatom] << "  ";
                    //  temporay constants for loop start and end indices
                    const int start = ::atoms::neighbour_list_start_index[iatom];
                    const int end = ::atoms::neighbour_list_end_index[iatom] + 1;

                    nbr_colours.assign(M, 0);
                    // loop over all neighbours and count how many of each colour
                    for (int nn = start; nn < end; ++nn)
                    {
                        const int atomj = ::atoms::neighbour_list_array[nn]; // get neighbouring atom number
                        int ac = atom_colour[atomj];
                        if (ac >= 0)
                            nbr_colours[ac]++;
                    }

                    // Go backwards through the number of nbring colours
                    // This atoms colour will be the lowest zero colour
                    int lowest = M;
                    for (int i = M - 1; i >= 0; i--)
                        if (nbr_colours[i] == 0)
                            lowest = i;

                    // If all there is no available colour increase the total and give the atom the new colour
                    if (lowest == M)
                        M++;

                    atom_colour[iatom] = lowest;
                    Ncoloured++;
                    // std::cout << "lowest = " << lowest << std::endl;

                    // Select the next atom from the next uncoloured neighbour
                    int next_atom = -1;
                    // loop over all neighbours
                    for (int nn = start; nn < end; ++nn)
                    {
                        const int atomj = ::atoms::neighbour_list_array[nn]; // get neighbouring atom number
                        int ac = atom_colour[atomj];
                        // find the lowest uncoloured nbr
                        if (ac < 0)
                        {
                            next_atom = atomj;
                            break;
                        }
                    }

                    // if all neighbours are coloured select from list
                    if (next_atom == -1)
                    {
                        for (int i = 0; i < N; i++)
                        {
                            int ac = atom_colour[i];
                            // find the lowest uncoloured nbr
                            if (ac < 0)
                            {
                                next_atom = i;
                                break;
                            }
                        }
                    }

                    // Move onto the next selected atom
                    iatom = next_atom;

                    // If we can't find another uncoloured atom, end the do while
                    if (next_atom == -1)
                    {
                        std::cout << "No more uncoloured atoms. Fraction coloured = " << Ncoloured << " / " << N << std::endl;
                        break;
                    }

                } while (Ncoloured < N);

                // Create a set of lists giving which atoms have which colours
                colour_list.resize(M);

                for (int i = 0; i < N; i++)
                {
                    int ac = atom_colour[i];
                    colour_list[ac].push_back(i);
                }

                for (int i = 0; i < M; i++)
                {
                    std::cout << "Colour: " << i << ", natoms = " << colour_list[i].size() << std::endl;
                }

                return 0;
            }

            int initialise()
            {

                curandStatus_t curand_status = curandCreateGenerator(&gen, CURAND_RNG_PSEUDO_DEFAULT);
                if (curand_status != CURAND_STATUS_SUCCESS)
                {
                    printf("CURAND create generator error: %d\n", curand_status);
                    exit(-1);
                }

                curand_status = curandSetPseudoRandomGeneratorSeed(gen, mtrandom::integration_seed);
                if (curand_status != CURAND_STATUS_SUCCESS)
                {
                    printf("CURAND set seed error: %d\n", curand_status);
                    exit(-1);
                }

                // Check for CUDA errors after CURAND operations
                cudaError_t curand_sync_error = cudaDeviceSynchronize();
                if (curand_sync_error != cudaSuccess)
                {
                    printf("CUDA error after CURAND setup at %s:%i: %s\n", __FILE__, __LINE__, cudaGetErrorString(curand_sync_error));
                    exit(-1);
                }

                cudaError_t malloc_error = cudaMalloc((void **)&d_rand_spin, 3 * ::atoms::num_atoms * sizeof(cu_real_t));
                if (malloc_error != cudaSuccess)
                {
                    printf("CUDA malloc error for d_rand_spin: %s\n", cudaGetErrorString(malloc_error));
                    exit(-1);
                }

                malloc_error = cudaMalloc((void **)&d_rand_accept, ::atoms::num_atoms * sizeof(cu_real_t));
                if (malloc_error != cudaSuccess)
                {
                    printf("CUDA malloc error for d_rand_accept: %s\n", cudaGetErrorString(malloc_error));
                    exit(-1);
                }

                malloc_error = cudaMalloc((void **)&d_sl_atoms, ::atoms::num_atoms * sizeof(int));
                if (malloc_error != cudaSuccess)
                {
                    printf("CUDA malloc error for d_sl_atoms: %s\n", cudaGetErrorString(malloc_error));
                    exit(-1);
                }
                malloc_error = cudaMalloc((void **)&d_accepted, ::atoms::num_atoms * sizeof(int));
                if (malloc_error != cudaSuccess)
                {
                    printf("CUDA malloc error for d_accepted: %s\n", cudaGetErrorString(malloc_error));
                    exit(-1);
                }

                // Check for CUDA errors after all memory allocations
                cudaError_t malloc_sync_error = cudaDeviceSynchronize();
                if (malloc_sync_error != cudaSuccess)
                {
                    printf("CUDA error after memory allocations at %s:%i: %s\n", __FILE__, __LINE__, cudaGetErrorString(malloc_sync_error));
                    exit(-1);
                }

                colour_split();

                // Check for CUDA errors after colour_split
                cudaError_t colour_sync_error = cudaDeviceSynchronize();
                if (colour_sync_error != cudaSuccess)
                {
                    printf("CUDA error after colour_split at %s:%i: %s\n", __FILE__, __LINE__, cudaGetErrorString(colour_sync_error));
                    exit(-1);
                }

                std::vector<int> h_sl_atoms;
                h_sl_atoms.resize(::atoms::num_atoms);
                int idx = 0;
                for (int i = 0; i < M; i++)
                {
                    sl_start.push_back(idx);
                    for (int j = 0; j < colour_list[i].size(); j++)
                    {
                        h_sl_atoms[idx] = colour_list[i][j];
                        idx++;
                    }
                }

                cudaError_t copy_error = cudaMemcpy(d_sl_atoms, h_sl_atoms.data(), ::atoms::num_atoms * sizeof(int), cudaMemcpyHostToDevice);
                if (copy_error != cudaSuccess)
                {
                    printf("CUDA memcpy error: %s\n", cudaGetErrorString(copy_error));
                    exit(-1);
                }

                // Check for CUDA errors immediately after memcpy
                cudaError_t sync_error = cudaDeviceSynchronize();
                if (sync_error != cudaSuccess)
                {
                    printf("CUDA error after memcpy at %s:%i: %s\n", __FILE__, __LINE__, cudaGetErrorString(sync_error));
                    exit(-1);
                }

                std::cout << "Trying a step..." << std::endl;

                // Use modern CUDA synchronization
                cudaDeviceSynchronize();
                cudaError_t error = cudaGetLastError();
                if (error != cudaSuccess)
                {
                    printf("CUDA error at %s:%i: %s\n", __FILE__, __LINE__, cudaGetErrorString(error));
                    exit(-1);
                }

                __mc_step();
                std::cout << "Done" << std::endl;

                initialised = true;

                return 0;
            }

            void finalise()
            {

                curandDestroyGenerator(gen);

                cudaFree(d_rand_spin);
                cudaFree(d_rand_accept);

                cudaFree(d_sl_atoms);
                cudaFree(d_accepted);
            }

            __global__ void monte_carlo_sublattice_step(
                const int sl_start,
                int *sl_atoms,
                int *material,
                vcuda::internal::material_parameters_t *material_params,
                cu_real_t *rand_spin,
                cu_real_t *rand_accept,
                int *accepted,
                cu_real_t *spin3n,
                cu_real_t *x_ext_field, cu_real_t *y_ext_field, cu_real_t *z_ext_field,
                cu_real_t *local_field_x, cu_real_t *local_field_y, cu_real_t *local_field_z, // 添加局部场参数
                int *csr_rows, int *csr_cols, cu_real_t *vals,
                const cu_real_t step_size, const cu_real_t global_temperature, const int N, const int Natoms,
                ::montecarlo::algorithm_t algorithm, cu_real_t adaptive_sigma)
            {

                // Loop over blocks for large systems > ~100k spins
                for (size_t i = blockIdx.x * blockDim.x + threadIdx.x;
                     i < N;
                     i += blockDim.x * gridDim.x)
                {

                    // load an atom from the current sublattice
                    int atom = sl_atoms[i + sl_start];

                    // Get material of atom
                    int mid = material[atom];

                    // Load parameters to local variables from memory
                    cu::material_parameters_t mat = material_params[mid];

                    // material dependent temperature rescaling
                    cu_real_t alpha = mat.temperature_rescaling_alpha;
                    cu_real_t Tc = mat.temperature_rescaling_Tc;
#ifdef CUDA_DP
                    cu_real_t rescaled_temperature = global_temperature < Tc ? Tc * pow(global_temperature / Tc, alpha) : global_temperature;
#else
                    cu_real_t rescaled_temperature = global_temperature < Tc ? Tc * __powf(global_temperature / Tc, alpha) : global_temperature;
#endif
                    cu_real_t rescaled_material_kBTBohr = 9.27400915e-24 / (rescaled_temperature * 1.3806503e-23);

#ifdef CUDA_DP
                    cu_real_t sigma = rescaled_temperature < 1.0 ? 0.02 : pow(1.0 / rescaled_material_kBTBohr, 0.2) * 0.08;
#else
                    cu_real_t sigma = rescaled_temperature < 1.0 ? 0.02 : __powf(1.0 / rescaled_material_kBTBohr, 0.2) * 0.08;
#endif

                    // load spin direction to registers for later multiple reuse
                    cu_real_t sx = spin3n[atom];
                    cu_real_t sy = spin3n[atom + Natoms];
                    cu_real_t sz = spin3n[atom + 2 * Natoms];

                    // new spin direction
                    cu_real_t nsx, nsy, nsz;

                    // run chosen move type
                    // Select algorithm using case statement
                    switch (algorithm)
                    {

                    case ::montecarlo::adaptive:
                    {
                        // TODO add in adaptive_sigma
                        nsx = sx + rand_spin[atom] * adaptive_sigma;
                        nsy = sy + rand_spin[atom + N] * adaptive_sigma;
                        nsz = sz + rand_spin[atom + 2 * N] * adaptive_sigma;

// find length using appropriate device sqrt function
#ifdef CUDA_DP
                        double mod_s = 1.0 / __dsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#else
                        float mod_s = 1.0 / __frsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#endif

                        nsx *= mod_s;
                        nsy *= mod_s;
                        nsz *= mod_s;
                        break;
                    }
                    case ::montecarlo::spin_flip:
                        nsx = -sx;
                        nsy = -sy;
                        nsz = -sz;
                        break;

                    case ::montecarlo::uniform:
                    {
                        nsx = rand_spin[atom];
                        nsy = rand_spin[atom + N];
                        nsz = rand_spin[atom + 2 * N];

// find length using appropriate device sqrt function
#ifdef CUDA_DP
                        double mod_s = 1.0 / __dsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#else
                        float mod_s = 1.0 / __frsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#endif

                        nsx *= mod_s;
                        nsy *= mod_s;
                        nsz *= mod_s;
                        break;
                    }

                    case ::montecarlo::angle:
                    {

                        nsx = sx + rand_spin[atom] * sigma;
                        nsy = sy + rand_spin[atom + N] * sigma;
                        nsz = sz + rand_spin[atom + 2 * N] * sigma;

// find length using appropriate device sqrt function
#ifdef CUDA_DP
                        double mod_s = 1.0 / __dsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#else
                        float mod_s = 1.0 / __frsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#endif

                        nsx *= mod_s;
                        nsy *= mod_s;
                        nsz *= mod_s;
                        break;
                    }

                    case ::montecarlo::hinzke_nowak:
                    {

                        // const int pick_move=int(3.0*mtrandom::grnd());
                        // mtrandom::grnd() is a random number in the half open interval [1,0)
                        int pick_move = 1;
                        switch (pick_move)
                        {
                        case 0: // spin flip
                            nsx = -sx;
                            nsy = -sy;
                            nsz = -sz;
                            break;
                        case 1: // uniform
                        {
                            nsx = rand_spin[atom];
                            nsy = rand_spin[atom + N];
                            nsz = rand_spin[atom + 2 * N];

// find length using appropriate device sqrt function
#ifdef CUDA_DP
                            double mod_s = 1.0 / __dsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#else
                            float mod_s = 1.0 / __frsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#endif

                            nsx *= mod_s;
                            nsy *= mod_s;
                            nsz *= mod_s;
                            break;
                        }

                        case 2: // angle
                        {

                            nsx = sx + rand_spin[atom] * sigma;
                            nsy = sy + rand_spin[atom + N] * sigma;
                            nsz = sz + rand_spin[atom + 2 * N] * sigma;

// find length using appropriate device sqrt function
#ifdef CUDA_DP
                            double mod_s = 1.0 / __dsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#else
                            float mod_s = 1.0 / __frsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#endif

                            nsx *= mod_s;
                            nsy *= mod_s;
                            nsz *= mod_s;
                            break;
                        }
                        default: // angle
                        {

                            nsx = sx + rand_spin[atom] * sigma;
                            nsy = sy + rand_spin[atom + N] * sigma;
                            nsz = sz + rand_spin[atom + 2 * N] * sigma;

// find length using appropriate device sqrt function
#ifdef CUDA_DP
                            double mod_s = 1.0 / __dsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#else
                            float mod_s = 1.0 / __frsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#endif

                            nsx *= mod_s;
                            nsy *= mod_s;
                            nsz *= mod_s;
                            break;
                        }
                        }
                        break;
                    }
                    default: // adaptive
                    {

                        // TODO add in adaptive_sigma
                        nsx = sx + rand_spin[atom] * adaptive_sigma;
                        nsy = sy + rand_spin[atom + N] * adaptive_sigma;
                        nsz = sz + rand_spin[atom + 2 * N] * adaptive_sigma;

// find length using appropriate device sqrt function
#ifdef CUDA_DP
                        double mod_s = 1.0 / __dsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#else
                        float mod_s = 1.0 / __frsqrt_rn(nsx * nsx + nsy * nsy + nsz * nsz);
#endif

                        nsx *= mod_s;
                        nsy *= mod_s;
                        nsz *= mod_s;
                        break;
                    }
                    }

                    // Calculate current energy
                    cu_real_t Eold = ::vcuda::internal::uniaxial_anisotropy_energy(mat, sx, sy, sz);
                    cu_real_t Enew = ::vcuda::internal::uniaxial_anisotropy_energy(mat, nsx, nsy, nsz);

                    cu_real_t hx = ::vcuda::internal::exchange::exchange_field_component(csr_cols, csr_rows, vals, spin3n, atom);
                    cu_real_t hy = ::vcuda::internal::exchange::exchange_field_component(csr_cols, csr_rows, vals, spin3n, atom + Natoms);
                    cu_real_t hz = ::vcuda::internal::exchange::exchange_field_component(csr_cols, csr_rows, vals, spin3n, atom + 2 * Natoms);

                    // ======唐愈涵加的目的是实现局部场======
                    // 添加局部场到交换场
                    hx += local_field_x[atom];
                    hy += local_field_y[atom];
                    hz += local_field_z[atom];
                    // ============================

                    cu_real_t dEx = -mat.mu_s_si * ((nsx - sx) * hx + (nsy - sy) * hy + (nsz - sz) * hz);

                    cu_real_t dE = (Enew - Eold + dEx) / (1.38064852e-23 * global_temperature);

                    cu_real_t r_accept = rand_accept[atom];

#ifdef CUDA_DP
                    if (r_accept < exp(-dE))
                    {
#else
                    if (r_accept < __expf(-dE))
                    {
#endif
                        spin3n[atom] = nsx;
                        spin3n[atom + Natoms] = nsy;
                        spin3n[atom + 2 * Natoms] = nsz;
                        accepted[i + sl_start] = 1;
                    }
                    // x_ext_field[atom] = rand_accept[atom];
                    // y_ext_field[atom] = rand_spin[atom];
                    // z_ext_field[atom] = rand_spin[atom+Natoms];
                    // x_ext_field[atom] = hx;
                    // y_ext_field[atom] = hy;
                    // z_ext_field[atom] = hz;
                }
            }

            void __mc_step()
            {
                // Check for cuda errors in file, line
                check_cuda_errors(__FILE__, __LINE__);

                // Load separate spin vectors into single array
                // cudaMemcpy(cu::exchange::d_spin3n, 				            cu::atoms::d_x_spin, ::atoms::num_atoms * sizeof(cu_real_t), cudaMemcpyDeviceToDevice);
                // cudaMemcpy(cu::exchange::d_spin3n + ::atoms::num_atoms, 	cu::atoms::d_y_spin, ::atoms::num_atoms * sizeof(cu_real_t), cudaMemcpyDeviceToDevice);
                // cudaMemcpy(cu::exchange::d_spin3n + 2 * ::atoms::num_atoms, cu::atoms::d_z_spin, ::atoms::num_atoms * sizeof(cu_real_t), cudaMemcpyDeviceToDevice);

                // generate 3 random doubles per atom for the trial spin and 1 for the acceptance
#ifdef CUDA_DP
                curandStatus_t status_normal = curandGenerateNormalDouble(gen, d_rand_spin, 3 * ::atoms::num_atoms, 0.0, 1.0);
                if (status_normal != CURAND_STATUS_SUCCESS)
                {
                    printf("CURAND error generating normal doubles: %d\n", status_normal);
                    exit(-1);
                }

                curandStatus_t status_uniform = curandGenerateUniformDouble(gen, d_rand_accept, ::atoms::num_atoms);
                if (status_uniform != CURAND_STATUS_SUCCESS)
                {
                    printf("CURAND error generating uniform doubles: %d\n", status_uniform);
                    exit(-1);
                }
#else
                curandStatus_t status_normal = curandGenerateNormal(gen, d_rand_spin, 3 * ::atoms::num_atoms, 0.0, 1.0);
                if (status_normal != CURAND_STATUS_SUCCESS)
                {
                    printf("CURAND error generating normal floats: %d\n", status_normal);
                    exit(-1);
                }
                curandStatus_t status_uniform = curandGenerateUniform(gen, d_rand_accept, ::atoms::num_atoms);
                if (status_uniform != CURAND_STATUS_SUCCESS)
                {
                    printf("CURAND error generating uniform floats: %d\n", status_uniform);
                    exit(-1);
                }
#endif

                cudaMemset(d_accepted, 0, ::atoms::num_atoms * sizeof(int));

                // Calculate external fields (fixed for integration step)
                // cu::update_external_fields ();

                // Iterate over all the sublattices
                for (int i = 0; i < M; i++)
                {
                    if (colour_list[i].size() > 0)
                    { // Only launch if there are atoms in this sublattice
                        monte_carlo_sublattice_step<<<cu::grid_size, cu::block_size>>>(
                            sl_start[i],
                            d_sl_atoms,
                            ::cu::atoms::d_materials, cu::mp::d_material_params,
                            d_rand_spin, d_rand_accept,
                            d_accepted,
                            ::cu::atoms::d_spin,
                            ::cu::d_x_external_field, ::cu::d_y_external_field, ::cu::d_z_external_field,
                            ::cu::local_field_x, ::cu::local_field_y, ::cu::local_field_z, // 添加局部场参数
                            ::vcuda::internal::exchange::d_csr_rows, ::vcuda::internal::exchange::d_coo_cols, ::vcuda::internal::exchange::d_coo_vals,
                            step_size, sim::temperature, colour_list[i].size(), ::atoms::num_atoms,
                            ::montecarlo::algorithm, (cu_real_t)::montecarlo::internal::adaptive_sigma);

                        // Check for kernel launch errors
                        cudaError_t kernel_error = cudaGetLastError();
                        if (kernel_error != cudaSuccess)
                        {
                            printf("CUDA kernel launch error for sublattice %d: %s\n", i, cudaGetErrorString(kernel_error));
                            exit(-1);
                        }
                    }
                    else
                    {
                        // Skip empty sublattice
                    }
                }

                // cudaMemcpy(cu::atoms::d_x_spin, cu::exchange::d_spin3n, 				            ::atoms::num_atoms * sizeof(cu_real_t), cudaMemcpyDeviceToDevice);
                // cudaMemcpy(cu::atoms::d_y_spin, cu::exchange::d_spin3n + ::atoms::num_atoms, 		::atoms::num_atoms * sizeof(cu_real_t), cudaMemcpyDeviceToDevice);
                // cudaMemcpy(cu::atoms::d_z_spin, cu::exchange::d_spin3n + 2 * ::atoms::num_atoms, 	::atoms::num_atoms * sizeof(cu_real_t), cudaMemcpyDeviceToDevice);

                // wrap raw pointer with a device_ptr
                thrust::device_ptr<int> dev_ptr(d_accepted);
                // total number of accepted moves
                int accepted_moves = thrust::reduce(dev_ptr, dev_ptr + ::atoms::num_atoms);
                int rejected_moves = ::atoms::num_atoms - accepted_moves;

                // calculate new adaptive step sigma angle
                if (::montecarlo::algorithm == ::montecarlo::adaptive)
                {
                    const cu_real_t last_rejection_rate = rejected_moves / ::atoms::num_atoms;
                    const cu_real_t factor = 0.5 / last_rejection_rate;
                    ::montecarlo::internal::adaptive_sigma *= factor;
                    // check for excessive range (too small angles takes too long to grow, too large doesn't improve performance) and truncate
                    if (::montecarlo::internal::adaptive_sigma > 60.0 || ::montecarlo::internal::adaptive_sigma < 1e-5)
                        montecarlo::internal::adaptive_sigma = 60.0;
                }

                // Save statistics to sim namespace variable
                sim::mc_statistics_moves += ::atoms::num_atoms;
                sim::mc_statistics_reject += rejected_moves;

                vcuda::transfer_spin_positions_from_gpu_to_cpu();
                // Output debugging
                // std::vector<double> hx, hy, hz;
                // hx.resize(::atoms::num_atoms);
                // hy.resize(::atoms::num_atoms);
                // hz.resize(::atoms::num_atoms);
                // cudaMemcpy(hx.data(), cu::d_x_external_field, ::atoms::num_atoms * sizeof(cu::cu_real_t), cudaMemcpyDeviceToHost);
                // cudaMemcpy(hy.data(), cu::d_y_external_field, ::atoms::num_atoms * sizeof(cu::cu_real_t), cudaMemcpyDeviceToHost);
                // cudaMemcpy(hz.data(), cu::d_z_external_field, ::atoms::num_atoms * sizeof(cu::cu_real_t), cudaMemcpyDeviceToHost);

                // std::cerr << hx[0] << "  " << hy[0] << "  " << hz[0] << std::endl;
            }
        }
    }
}
