// What does including a CUDA header do to an otherwise ordinary translation unit?
#include <cuda_runtime.h>

#ifdef __CUDACC__
#  pragma message("__CUDACC__ is defined")
#endif
#ifdef __CUDACC_VER_MAJOR__
#  pragma message("__CUDACC_VER_MAJOR__ is defined")
#endif
#ifdef __NVCC__
#  pragma message("__NVCC__ is defined")
#endif
#ifdef __CUDA_ARCH__
#  pragma message("__CUDA_ARCH__ is defined in the host pass too")
#endif

// The annotations that are not C++: valid only because the frontend was
// replaced, and meaningless to any other compiler reading the same file.
__host__ __device__ int both_sides(int x) { return x + 1; }
__global__ void kernel(int* p) { *p = both_sides(threadIdx.x); }

int main() { return 0; }
