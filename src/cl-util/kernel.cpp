#include "kernel.h"
#include <cassert>
#include <iostream>
#include "util/exceptions.h"
#include "util/string-util.h"
using namespace std;

namespace fract { namespace CL {

Kernel::Kernel(string file_name, string params,
  string kernel_name, shared_ptr<Context> context
) {
  string text = ReadFile(file_name);

  Init(text, params, kernel_name, context);
}

Kernel::Kernel(string include_dir, string file_name, string params,
  string kernel_name, shared_ptr<Context> context
) {
	params += " -I ";
	params += include_dir; // TODO: escape spaces
	string text = "#include \"" + file_name + "\"\n";

	Init(text, params, kernel_name, context);
}

void Kernel::Init(string text, string params,
  string kernel_name, shared_ptr<Context> context
) {
	assert(context);
  context_ = context;

	int err;

  const char *text_c = text.c_str();
  program_ = clCreateProgramWithSource(
    context->context(), 1, &text_c, NULL, &err);
  if (!program_ || err != CL_SUCCESS)
    throw CLException("failed to create program");
  
  cl_device_id device_id = context_->device_id();
  int build_err = clBuildProgram(
    program_, 1, &device_id, params.c_str(), NULL, NULL);

	size_t len;
  static char buffer[65536];
  
  err = clGetProgramBuildInfo(program_, device_id, CL_PROGRAM_BUILD_LOG,
    sizeof(buffer) - 1, buffer, &len);
  if (err != CL_SUCCESS)
    throw CLException("failed to get program build log");
  buffer[len] = 0;
  std::cerr << buffer << std::endl;

  if (build_err != CL_SUCCESS)
    throw CLException("failed to build program");

  kernel_ = clCreateKernel(program_, kernel_name.c_str(), &err);
  if (!kernel_ || err != CL_SUCCESS)
    throw CLException("failed to create compute kernel");

  size_t wgsize;
  err = clGetKernelWorkGroupInfo(kernel_, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &wgsize, NULL);
  if (err != CL_SUCCESS)
    throw CLException("failed to retrieve kernel work group info");
  
  work_group_size_ = static_cast<int>(wgsize);
}

Kernel::~Kernel() {
  clReleaseKernel(kernel_);
  clReleaseProgram(program_);
}

int Kernel::work_group_size() const {
	return work_group_size_;
}

cl_kernel Kernel::kernel() const {
  return kernel_;
}

void Kernel::SetArg(int index, int size, void *data) {
  assert(index >= 0);
  assert(size > 0);
  int err = clSetKernelArg(kernel_, index, size, data);
  if (err != CL_SUCCESS)
    throw CLException("failed to set kernel arg");
}

void Kernel::SetLocalBufferArg(int index, int size) {
  SetArg(index, size, NULL);
}

void Kernel::SetIntArg(int index, int val) {
  SetArg(index, sizeof(cl_int), &val);
}

void Kernel::SetFloatArg(int index, float val) {
  SetArg(index, sizeof(cl_float), &val);
}

void Kernel::SetFloat4Arg(int index, fvec3 xyz, float w) {
	float v[4] = { xyz.x, xyz.y, xyz.z, w };
	SetArg(index, 4 * sizeof(cl_float), v);
}

void Kernel::SetBufferArg(int index, const Buffer *buf) {
	cl_mem mem;
	if (buf)
		mem = buf->buffer();
	else
		mem = NULL;
    SetArg(index, sizeof(cl_mem), &mem);
}

void Kernel::SetFloat16Arg(int index, fmat4 mat) {
  SetArg(index, sizeof(cl_float16), mat.m);
}

}}
