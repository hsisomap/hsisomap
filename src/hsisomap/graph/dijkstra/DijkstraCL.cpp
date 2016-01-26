//
// Created by Can on 3/14/15.
//

#include <numeric>
#include <vector>
#include <hsisomap/graph/dijkstra/DijkstraCL.h>

namespace Dijkstra {

//! checkError macro to incorporate the line of code with error for convenient debugging.
#define checkError() checkErrorFileLine(__FILE__ , __LINE__)

const char *programSource = R"(
__kernel void OCL_SSSP_KERNEL1(__global int *vertices, __global int *edges, __global float *weights,
                               __global int *masks, __global float *costs, __global float *updatingCosts,
                               int vertexCount, int edgeCount ) {
  int tid = get_global_id(0);

  if (masks[tid] != 0) {
    masks[tid] = 0;

    int edgeStart = vertices[tid];
    int edgeEnd;
    if (tid + 1 < (vertexCount)) {
      edgeEnd = vertices[tid + 1];
    } else {
      edgeEnd = edgeCount;
    }

    for(int edge = edgeStart; edge < edgeEnd; edge++) {
      int nid = edges[edge];
      if (updatingCosts[nid] > (costs[tid] + weights[edge])) {
        updatingCosts[nid] = (costs[tid] + weights[edge]);
      }
    }
  }
}

__kernel void OCL_SSSP_KERNEL2(__global int *vertices, __global int *edges, __global float *weights,
                               __global int *masks, __global float *costs, __global float *updatingCosts,
                               int vertexCount) {
  int tid = get_global_id(0);

  if (costs[tid] > updatingCosts[tid]) {
    costs[tid] = updatingCosts[tid];
    masks[tid] = 1;
  }

  updatingCosts[tid] = costs[tid];
}

__kernel void initializeBuffers(__global int *masks, __global float *costs, __global float *updatingCosts,
                                int sourceVertex, int vertexCount ) {
  int tid = get_global_id(0);

  if (sourceVertex == tid) {
    masks[tid] = 1;
    costs[tid] = 0.0;
    updatingCosts[tid] = 0.0;
  } else {
    masks[tid] = 0;
    costs[tid] = FLT_MAX;
    updatingCosts[tid] = FLT_MAX;
  }
}
)";

DijkstraCL::DijkstraCL(std::shared_ptr<GraphUtils::AdjacencyList> adjList)
    : graph_(adjList->GetGraphArray<cl_Index, cl_Scalar>()),
      numVertices_(static_cast<cl_Index>(graph_->vertices.size())),
      sourceVertices_(graph_->vertices.size(), 0),
      lastErr_(CL_SUCCESS) {
  std::iota(sourceVertices_.begin(), sourceVertices_.end(), 0);
}

int DijkstraCL::Run() {
  // Allocate results array
  results_.reset(new std::vector<cl_Scalar>(sourceVertices_.size() * graph_->vertices.size(), 0));
  lastErr_ = InitializeDevices();
  if (lastErr_ != CL_SUCCESS) return lastErr_;
  RunOnDevice(context_, device_);
  return static_cast<int>(lastErr_);
}

void DijkstraCL::SetSourceVertices(std::vector<Index> sourceVertices) {
  sourceVertices_.resize(sourceVertices.size());
  std::copy(sourceVertices.begin(), sourceVertices.end(), sourceVertices_.begin());
}

cl_int DijkstraCL::InitializeDevices() {
  // Find the first OpenCL Plaform
  cl_uint numPlatforms;
  lastErr_ = clGetPlatformIDs(1, &platform_, &numPlatforms);
  if (lastErr_ != CL_SUCCESS || numPlatforms <= 0) return lastErr_;

  context_ = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, &lastErr_);
  if (lastErr_ != CL_SUCCESS) return lastErr_;
  device_ = GetMaxFlopsDevice();

  return lastErr_;
}

cl_device_id DijkstraCL::GetMaxFlopsDevice() {
  size_t parameterSize;
  cl_device_id *devices;
  clGetContextInfo(context_, CL_CONTEXT_DEVICES, 0, NULL, &parameterSize);
  cl_device_id maxFlopsDevice;

  {
    devices = (cl_device_id *) malloc(parameterSize);
    size_t deviceCount = parameterSize / sizeof(cl_device_id);
    clGetContextInfo(context_, CL_CONTEXT_DEVICES, parameterSize, devices, NULL);

    maxFlopsDevice = devices[0];
    int maxFlops = 0;
    size_t currentDevice = 0;
    cl_uint computeUnits;
    clGetDeviceInfo(devices[currentDevice], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, NULL);
    cl_uint clockFrequency;
    clGetDeviceInfo(devices[currentDevice],
                    CL_DEVICE_MAX_CLOCK_FREQUENCY,
                    sizeof(clockFrequency),
                    &clockFrequency,
                    NULL);
    maxFlops = computeUnits * clockFrequency;

    while (currentDevice < deviceCount) {
      cl_uint computeUnits;
      clGetDeviceInfo(devices[currentDevice], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, NULL);
      cl_uint clockFrequency;
      clGetDeviceInfo(devices[currentDevice],
                      CL_DEVICE_MAX_CLOCK_FREQUENCY,
                      sizeof(clockFrequency),
                      &clockFrequency,
                      NULL);
      int flops = computeUnits * clockFrequency;
      if (flops > maxFlops) {
        maxFlops = flops;
        maxFlopsDevice = devices[currentDevice];
      }
      currentDevice++;
    }

    free(devices);
    return maxFlopsDevice;
  }
}

void DijkstraCL::checkErrorFileLine(const char *file, const int lineNumber) {
  if (lastErr_ != CL_SUCCESS) {
    std::cerr << "Line " << lineNumber << " in File " << file << std::endl;
    exit(1);
  }
}

cl_int DijkstraCL::RunOnDevice(cl_context context, cl_device_id device) {
  // Create command queue
  cl_command_queue commandQueue;
  commandQueue = clCreateCommandQueue(context, device, 0, &lastErr_); checkError();

  // Load program
  cl_program program = buildProgram(context);

  // Get the maximum workgroup size
  size_t maxWorkGroupSize;
  clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL); checkError();

  // Set work items as 1 dimension
  size_t localWorkSize = maxWorkGroupSize;
  size_t globalWorkSize = RoundUpWorkSize(localWorkSize, graph_->vertices.size());

  cl_mem verticesDevice;
  cl_mem edgesDevice;
  cl_mem weightsDevice;
  cl_mem maskDevice;
  cl_mem costDevice;
  cl_mem updatingCostDevice;

  // Allocate and copy data into device
  PrepareDeviceMemory(context, commandQueue, *graph_, &verticesDevice, &edgesDevice, &weightsDevice,
                      &maskDevice, &costDevice, &updatingCostDevice, globalWorkSize);

  // Convert rvalue to local variable so that an address can be taken
  // The clSetKernelArg takes pointers even for plain type arguments
  cl_Index vertexCount = graph_->vertices.size();
  cl_Index edgeCount = graph_->edges.size();

  // Create initializeBuffers Kernel and set args
  cl_kernel initializeBuffersKernel = clCreateKernel(program, "initializeBuffers", &lastErr_); checkError();
  lastErr_ |= clSetKernelArg(initializeBuffersKernel, 0, sizeof(cl_mem), &maskDevice);
  lastErr_ |= clSetKernelArg(initializeBuffersKernel, 1, sizeof(cl_mem), &costDevice);
  lastErr_ |= clSetKernelArg(initializeBuffersKernel, 2, sizeof(cl_mem), &updatingCostDevice);

  // 3rd Arg is changing for each source vertex and set in the loop below
  lastErr_ |= clSetKernelArg(initializeBuffersKernel, 4, sizeof(cl_Index), &vertexCount); checkError();


  // Create SSSP Kernel 1 and set args
  cl_kernel ssspKernel1;
  ssspKernel1 = clCreateKernel(program, "OCL_SSSP_KERNEL1", &lastErr_); checkError();
  lastErr_ |= clSetKernelArg(ssspKernel1, 0, sizeof(cl_mem), &verticesDevice);
  lastErr_ |= clSetKernelArg(ssspKernel1, 1, sizeof(cl_mem), &edgesDevice);
  lastErr_ |= clSetKernelArg(ssspKernel1, 2, sizeof(cl_mem), &weightsDevice);
  lastErr_ |= clSetKernelArg(ssspKernel1, 3, sizeof(cl_mem), &maskDevice);
  lastErr_ |= clSetKernelArg(ssspKernel1, 4, sizeof(cl_mem), &costDevice);
  lastErr_ |= clSetKernelArg(ssspKernel1, 5, sizeof(cl_mem), &updatingCostDevice);
  lastErr_ |= clSetKernelArg(ssspKernel1, 6, sizeof(cl_Index), &vertexCount);
  lastErr_ |= clSetKernelArg(ssspKernel1, 7, sizeof(cl_Index), &edgeCount); checkError();

  // Create SSSP Kernel 2 and set args
  cl_kernel ssspKernel2;
  ssspKernel2 = clCreateKernel(program, "OCL_SSSP_KERNEL2", &lastErr_); checkError();
  lastErr_ |= clSetKernelArg(ssspKernel2, 0, sizeof(cl_mem), &verticesDevice);
  lastErr_ |= clSetKernelArg(ssspKernel2, 1, sizeof(cl_mem), &edgesDevice);
  lastErr_ |= clSetKernelArg(ssspKernel2, 2, sizeof(cl_mem), &weightsDevice);
  lastErr_ |= clSetKernelArg(ssspKernel2, 3, sizeof(cl_mem), &maskDevice);
  lastErr_ |= clSetKernelArg(ssspKernel2, 4, sizeof(cl_mem), &costDevice);
  lastErr_ |= clSetKernelArg(ssspKernel2, 5, sizeof(cl_mem), &updatingCostDevice);
  lastErr_ |= clSetKernelArg(ssspKernel2, 6, sizeof(cl_Index), &vertexCount); checkError();

  cl_Index *maskHost = (cl_Index *) malloc(sizeof(cl_Index) * vertexCount);

  for (cl_Index i = 0 ; i < sourceVertices_.size(); i++ ) {
    cl_event event;

    // Set source vertex as the 3rd arg of initializeBuffer kernel
    lastErr_ = clSetKernelArg(initializeBuffersKernel, 3, sizeof(cl_Index), &sourceVertices_[i]); checkError();

    // Initialize mask array to false, C and U to infinity
    lastErr_ = clEnqueueNDRangeKernel(commandQueue, initializeBuffersKernel, 1, NULL, &globalWorkSize, NULL,
                                      0, NULL, &event); checkError();
    clWaitForEvents(1, &event); // Sync

//--- For Debug
//    lastErr_ = clEnqueueReadBuffer(commandQueue, maskDevice, CL_FALSE, 0, sizeof(cl_Index) * vertexCount,
//                                   maskHost, 0, NULL, &event); checkError();
//    clWaitForEvents(1, &event);
//    std::cout << "> " << i << "]"; for (int a = 0; a < vertexCount; ++a) std::cout << maskHost[a] << "-";
//    std::cout << std::endl;
//---

    // Read mask array from device to host
    lastErr_ = clEnqueueReadBuffer(commandQueue, maskDevice, CL_FALSE, 0, sizeof(cl_Index) * vertexCount,
                                   maskHost, 0, NULL, &event); checkError();
    lastErr_ = clWaitForEvents(1, &event); checkError(); // Sync

    while(!isMaskEmpty(maskHost, vertexCount)) {

      // Number of iterations before reading results to prevent stalling if data set is small; From original OpenCL Programming Guide.
#define DIJKSTRACL_GPU_ITERATIONS_PREVENT_STALLING 1
      for(int iteration = 0; iteration < DIJKSTRACL_GPU_ITERATIONS_PREVENT_STALLING; ++iteration) {
        // execute the kernel
        lastErr_ = clEnqueueNDRangeKernel(commandQueue, ssspKernel1, 1, 0, &globalWorkSize, NULL,
                                          0, NULL, NULL); checkError();

        lastErr_ = clEnqueueNDRangeKernel(commandQueue, ssspKernel2, 1, 0, &globalWorkSize, NULL,
                                          0, NULL, NULL); checkError();
      }
      lastErr_ = clEnqueueReadBuffer(commandQueue, maskDevice, CL_FALSE, 0, sizeof(cl_Index) * vertexCount,
                                     maskHost, 0, NULL, &event); checkError();
      clWaitForEvents(1, &event);
    }

    // Get results
    lastErr_ = clEnqueueReadBuffer(commandQueue, costDevice, CL_FALSE, 0, sizeof(cl_Scalar) * vertexCount,
                                   &((*results_)[i * vertexCount]), 0, NULL, &event); checkError();
    clWaitForEvents(1, &event); // Sync
  }

  free(maskHost);

  clReleaseMemObject(verticesDevice);
  clReleaseMemObject(edgesDevice);
  clReleaseMemObject(weightsDevice);
  clReleaseMemObject(maskDevice);
  clReleaseMemObject(costDevice);
  clReleaseMemObject(updatingCostDevice);

  clReleaseKernel(initializeBuffersKernel);
  clReleaseKernel(ssspKernel1);
  clReleaseKernel(ssspKernel2);

  clReleaseCommandQueue(commandQueue);
  clReleaseProgram(program);
  return lastErr_;
}

cl_program DijkstraCL::buildProgram(cl_context context) {
  // Create program for all devices in the context
  cl_program program = clCreateProgramWithSource(context, 1, (const char **) &programSource, NULL, &lastErr_); checkError();
  // Build program
  lastErr_ = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (lastErr_ != CL_SUCCESS) {
    char buildLog[10240];
    clGetProgramBuildInfo(program, device_, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL); checkError();
    std::cerr << buildLog << std::endl;
  }
  return program;
}

size_t DijkstraCL::RoundUpWorkSize(size_t groupSize, size_t globalSize) {
  size_t remainder = globalSize % groupSize;
  if (remainder == 0) return globalSize;
  return globalSize + groupSize - remainder;
}

void DijkstraCL::PrepareDeviceMemory(cl_context context,
                                     cl_command_queue commandQueue,
                                     GraphUtils::GraphArray<cl_Index, cl_Scalar> &graph,
                                     cl_mem *verticesDevice,
                                     cl_mem *edgesDevice,
                                     cl_mem *weightsDevice,
                                     cl_mem *maskDevice,
                                     cl_mem *costDevice,
                                     cl_mem *updatingCostDevice,
                                     size_t globalWorkSize) {
  cl_mem verticesHost;
  cl_mem edgesHost;
  cl_mem weightsHost;

  // Create host buffer from host data pointers
  verticesHost = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR,
                                sizeof(cl_Index) * graph.vertices.size(), &graph.vertices[0], &lastErr_); checkError();

  edgesHost = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR,
                             sizeof(cl_Index) * graph.edges.size(), &graph.edges[0], &lastErr_); checkError();

  weightsHost = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR | CL_MEM_ALLOC_HOST_PTR,
                               sizeof(cl_Scalar) * graph.weights.size(), &graph.weights[0], &lastErr_); checkError();

  // Create all device buffer
  *verticesDevice = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_Index) * globalWorkSize, NULL, &lastErr_); checkError();
  *edgesDevice = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_Index) * graph.edges.size(), NULL, &lastErr_); checkError();
  *weightsDevice = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_Scalar) * graph.weights.size(), NULL, &lastErr_); checkError();
  *maskDevice = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_Index) * globalWorkSize, NULL, &lastErr_); checkError();
  *costDevice = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_Scalar) * globalWorkSize, NULL, &lastErr_); checkError();
  *updatingCostDevice = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_Scalar) * globalWorkSize, NULL, &lastErr_); checkError();

  // Enqueue the data reading commands
  cl_event event;

  lastErr_ = clEnqueueCopyBuffer(commandQueue, verticesHost, *verticesDevice, 0, 0,
                                 sizeof(cl_Index) * graph.vertices.size(), 0, NULL, &event); checkError();
  clWaitForEvents(1, &event);

  lastErr_ = clEnqueueCopyBuffer(commandQueue, edgesHost, *edgesDevice, 0, 0,
                                 sizeof(cl_Index) * graph.edges.size(), 0, NULL, &event); checkError();
  clWaitForEvents(1, &event);

  lastErr_ = clEnqueueCopyBuffer(commandQueue, weightsHost, *weightsDevice, 0, 0,
                                 sizeof(cl_Scalar) * graph.weights.size(), 0, NULL, &event); checkError();
  clWaitForEvents(1, &event);

  // Release host memory objects
  clReleaseMemObject(verticesHost);
  clReleaseMemObject(edgesHost);
  clReleaseMemObject(weightsHost);
}

std::shared_ptr<std::vector<DijkstraCL::cl_Scalar>> DijkstraCL::GetResultsArray() {
  return results_;
}

bool DijkstraCL::isMaskEmpty(DijkstraCL::cl_Index *mask, DijkstraCL::cl_Index maskSize) {
  for(cl_Index i = 0; i < maskSize; i++ ) {
    if (mask[i] == 1) return false;
  }
  return true;
}

std::shared_ptr<gsl::Matrix> DijkstraCL::GetDistanceMatrix() {
  std::shared_ptr<gsl::Matrix> result(new gsl::Matrix(sourceVertices_.size(), static_cast<Index>(numVertices_)));
  for (Index r = 0; r < sourceVertices_.size(); ++r) {
    for (Index c = 0; c < numVertices_; ++c) {
      Scalar val = static_cast<Scalar>((*results_)[r * numVertices_ + c]);
      // Note that the float max value is directly converted to double max value for convenience.
      // This is assuming the client code is aware of possible imprecision:
      // e.g., if client code use double as Scalar, and OpenCL device only supports float as cl_Scalar,
      // then cl_Scalar overflow doesn't mean a Scalar overflow.
      // However, this is ok for almost all applications since the "overflow" here just means no connection.
      if (val == std::numeric_limits<cl_Scalar>::max()) val = std::numeric_limits<Scalar>::max();
      (*result)(r, c) = val;
    }
  }
  return result;
}

}