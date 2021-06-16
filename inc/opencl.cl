__kernel void vector_add(__global const int *A, __global const int *B, __global int *C) {
    int i = get_global_id(0); // Get the index of the current element to be processed
    C[i] = A[i] + B[i]; // Do the operation
}
