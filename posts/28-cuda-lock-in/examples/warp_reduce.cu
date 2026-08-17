// Spike: what can Compiler Explorer actually show about the CUDA toolchain?
// One kernel that exercises several of the claims at once:
//   - a warp-level primitive (__shfl_down_sync), the thing that does not port
//   - enough arithmetic that PTX and SASS differ visibly
#include <cstdio>

__global__ void warp_reduce(const float* in, float* out, int n) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    float v = (tid < n) ? in[tid] : 0.0f;

    // Warp-level reduction. This is the coupling: __shfl_down_sync is an
    // Nvidia warp primitive, and 32 is the warp width it assumes.
    for (int offset = 16; offset > 0; offset >>= 1) {
        v += __shfl_down_sync(0xffffffffu, v, offset);
    }

    if ((threadIdx.x & 31) == 0) {
        atomicAdd(out, v);
    }
}
