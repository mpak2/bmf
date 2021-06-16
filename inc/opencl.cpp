//#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <CL/cl2.hpp>
#include <iostream>
#include <fstream>
//#include <vector>
#include <string>

int main(){
   /*std::vector<cl::Platform> platforms;
   cl::Platform::get(&platforms);

	std::cout << "Платформ " << platforms.size() << std::endl;
	for(auto platform:platforms){
		std::cout << "Платформа :" << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
		for(auto device:devices){
			std::cout << "\tОборудование :" << device.getInfo<CL_DEVICE_NAME>() << std::endl;
		}
	}*/


	printf("\nСтарт расчётов\n");

	// Create the two input vectors
	int i;
	const int LIST_SIZE = 1000000;
	int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
	int *B = (int*)malloc(sizeof(int)*LIST_SIZE);
	for(i = 0; i < LIST_SIZE; i++) {
		A[i] = i;
		B[i] = LIST_SIZE - i;
	}

	char *source_str;
	size_t source_size;

	if(FILE *fp = fopen("opencl.cl", "r"); !fp){ fprintf(stderr, "ОШИБКА загрузки ядра.\n");
	}else{
		int max_source_size = 0x100000;
		source_str = (char*)malloc(max_source_size);
		source_size = fread( source_str, 1, max_source_size, fp);
		fclose(fp);
		printf("Ядро загружено\n");

		cl_device_id device_id = NULL; // Идентификатор оборудования
		cl_uint ret_num_devices; // Оборудование
		cl_uint ret_num_platforms; // Платформа


		cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
		cl_platform_id *platforms = NULL;
		platforms = (cl_platform_id*)malloc(ret_num_platforms*sizeof(cl_platform_id));

		ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL); //printf("ret at %d is %d\n", __LINE__, ret);
		ret = clGetDeviceIDs( platforms[1], CL_DEVICE_TYPE_ALL, 1, &device_id, &ret_num_devices); //printf("ret at %d is %d\n", __LINE__, ret);
		cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret); //printf("ret at %d is %d\n", __LINE__, ret);

		cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret); //printf("ret at %d is %d\n", __LINE__, ret);

		cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);
		cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);
		cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int), NULL, &ret); // Copy the lists A and B to their respective memory buffers

		ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), A, 0, NULL, NULL); //printf("ret at %d is %d\n", __LINE__, ret);
		ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), B, 0, NULL, NULL); //printf("ret at %d is %d\n", __LINE__, ret);

		cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret); //printf("ret at %d is %d\n", __LINE__, ret);
		ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL); //printf("ret at %d is %d\n", __LINE__, ret);

		cl_kernel kernel = clCreateKernel(program, "vector_add", &ret); //printf("ret at %d is %d\n", __LINE__, ret);
		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj); //printf("ret at %d is %d\n", __LINE__, ret);
		ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj); //printf("ret at %d is %d\n", __LINE__, ret);
		ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj); //printf("ret at %d is %d\n", __LINE__, ret);

		//added this to fix garbage output problem
		//ret = clSetKernelArg(kernel, 3, sizeof(int), &LIST_SIZE);

		size_t global_item_size = LIST_SIZE; // Process the entire lists
		size_t local_item_size = 256; // Divide work items into groups of 64
		ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL); //printf("after execution\n");

		// Read the memory buffer C on the device to the local variable C
		int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
		ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), C, 0, NULL, NULL); //printf("after copying\n");

		 // Display the result to the screen
		/*for(i = 0; i < LIST_SIZE; i++){
			 printf("%d + %d = %d\n", A[i], B[i], C[i]);
		}*/
		printf("Размер списка расчетов = %d", LIST_SIZE);


		// Clean up
		ret = clFlush(command_queue);
		ret = clFinish(command_queue);
		ret = clReleaseKernel(kernel);
		ret = clReleaseProgram(program);
		ret = clReleaseMemObject(a_mem_obj);
		ret = clReleaseMemObject(b_mem_obj);
		ret = clReleaseMemObject(c_mem_obj);
		ret = clReleaseCommandQueue(command_queue);
		ret = clReleaseContext(context);
		free(A);
		free(B);
		free(C);
	}

	return 0;
}
