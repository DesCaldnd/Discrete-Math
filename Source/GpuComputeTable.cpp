//
// Created by ivanl on 22.03.2023.
//

#include "../Headers/GpuComputeTable.h"
#include <QDebug>
#include <string>
#include <QString>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

GPUComputeTable::GPUComputeTable()
{
	cl::Platform platform;

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	qDebug() << "Selected platform:\n";
	for (auto &p : platforms)
	{
		std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
		if (platver.find("OpenCL 2.") != std::string::npos ||
			platver.find("OpenCL 3.") != std::string::npos)
		{
			std::string name = p.getInfo<CL_PLATFORM_NAME>();
			std::string vendor = p.getInfo<CL_PLATFORM_VENDOR>();
			std::string profile = p.getInfo<CL_PLATFORM_PROFILE>();
			std::string extensions = p.getInfo<CL_PLATFORM_EXTENSIONS>();
			qDebug() << "Name: " << QString::fromStdString(name) << "\nVendor: " << QString::fromStdString(vendor)
					 << "\nProfile: " << QString::fromStdString(profile)
					 << "\nExtensions: " << QString::fromStdString(extensions) << "\n";
			platform = p;
			break;
		}
	}

	if (platform() == 0)
		throw std::runtime_error("No platform found");

	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, &devices);
	if (devices.empty())
		throw std::runtime_error("No devices found");
	cl::Device device = devices[0];

	qDebug() << "Selected device:\n";

	int available = device.getInfo<CL_DEVICE_AVAILABLE>();
	std::string name = device.getInfo<CL_DEVICE_NAME>();
	unsigned int compiler_available = device.getInfo<CL_DEVICE_COMPILER_AVAILABLE>();
	std::string vendor = device.getInfo<CL_DEVICE_VENDOR>();
	std::string version = device.getInfo<CL_DEVICE_VERSION>();
	std::string extensions = device.getInfo<CL_DEVICE_EXTENSIONS>();
	std::string profile = device.getInfo<CL_DEVICE_PROFILE>();
	unsigned long long global_mem_size = device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
	local_mem_size = device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
	unsigned int frequency = device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
	std::string opencl_c_version = device.getInfo<CL_DEVICE_OPENCL_C_VERSION>();
	max_group_size = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
	int max_compute_units = device.getInfo<	CL_DEVICE_MAX_COMPUTE_UNITS>();
	int max_dims = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>();
	qDebug() << "Name: " << QString::fromStdString(name)
			 << "\nAvailable: " << QString::number(available)
			 << "\nVendor: " << QString::fromStdString(vendor)
			 << "\nProfile: " << QString::fromStdString(profile)
			 << "\nCompiler available" << QString::number(compiler_available)
			 << "\nDevice version: " << QString::fromStdString(version)
			 << "\nGlobal memory size: " << QString::number(global_mem_size)
			 << "\nLocal memory size: " << QString::number(local_mem_size)
			 << "\nMax work group size: " << QString::number(max_group_size)
			 << "\nMax compute_module units: " << QString::number(max_compute_units)
			 << "\nFrequency: " << QString::number(frequency)
			 << "\nOpenCL C version: " << QString::fromStdString(opencl_c_version)
			 << "\nMax dimensions " << QString::number(max_dims)
			 << "\nExtensions: " << QString::fromStdString(extensions)
			 << "\n";

	cl_context_properties properties[] = {
		CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform()),
		0 // signals end of property list
	};
	context_ = std::make_unique<cl::Context>(CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, properties);

	std::ifstream kernel_file("../Kernels/TableComputeKernel.cl");
	std::ostringstream sstr;
	sstr << kernel_file.rdbuf();
	std::string code = sstr.str();
	kernel_file.close();

	cl::Program::Sources sources;
	sources.push_back({code.c_str(), code.length()});
	try
	{
		program_ = std::make_unique<cl::Program>(*context_, sstr.str(), false);
		program_->build("-cl-std=CL2.0");
	} catch (cl::BuildError &err) {
		std::cerr << "OCL BUILD ERROR: " << err.err() << ":" << err.what()
				  << std::endl;
		std::cerr << "-- Log --\n";
		for (auto e : err.getBuildLog())
			std::cerr << e.second;
		std::cerr << "-- End log --\n";
		throw;
	} catch (cl::Error &err) {
		std::cerr << "OCL ERROR: " << err.err() << ":" << err.what() << std::endl;
		throw;
	} catch (std::runtime_error &err) {
		std::cerr << "RUNTIME ERROR: " << err.what() << std::endl;
		throw;
	} catch (...) {
		std::cerr << "UNKNOWN ERROR\n";
		throw;
	}
	qDebug() << "Kernel code: " << QString::fromStdString(code);

	command_queue_ = std::make_unique<cl::CommandQueue>(*context_, device);

	process_vecs = std::make_unique<cl::KernelFunctor<cl::Buffer, int, cl::Buffer, int, int, cl::Buffer, cl::Buffer,
					  cl::LocalSpaceArg, cl::LocalSpaceArg>>(*program_, "compute_table");
}

void GPUComputeTable::run(std::vector<bool> &result, const std::vector<Variable> &variables,
						  const std::vector<std::shared_ptr<ExpressionSymbol>> &expression, const int operCount, unsigned int &trues)
{
	trues = 0;
	unsigned int vars_2 = power_of_2(variables.size());
	unsigned int max_local_group_size_on_memory = align(local_mem_size / (sizeof(ES) * (variables.size() + expression.size() - operCount)));
	unsigned int group_size = std::min(max_local_group_size_on_memory, std::min(vars_2, max_group_size));
	qDebug() << "Max on memory: " << max_local_group_size_on_memory;
//	qDebug() << "Bottom: " << sizeof(ES) * (variables.size() + expression.size() - operCount);
	qDebug() << "Group size: " << group_size;
	cl::Buffer vars_buf(*context_, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(ES) * variables.size());
	cl::Buffer expr_buf(*context_, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(ES) * expression.size());
	cl::Buffer trues_buf(*context_, CL_MEM_WRITE_ONLY, sizeof(unsigned int));
	cl::Buffer answer_buf(*context_, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, sizeof(bool) * vars_2 * (variables.size() + operCount));

	std::vector<ES> struct_vars{variables.size()};
	std::vector<ES> struct_expr{expression.size()};

	for(int i = 0; i < variables.size(); ++i)
	{
		struct_vars[i].Symbol = variables[i].getSymbol();
		struct_vars[i].type = ES::Type::Var;
	}
	for(int i = 0; i < expression.size(); ++i)
	{
		struct_expr[i].Symbol = expression[i]->getSymbol();
		struct_expr[i].type = (ES::Type)(int)expression[i]->getType();
	}

	cl::copy(*command_queue_, struct_vars.begin(), struct_vars.end(), vars_buf);
	cl::copy(*command_queue_, struct_expr.begin(), struct_expr.end(), expr_buf);
	cl::copy(*command_queue_, &trues, (&trues) + 1, trues_buf);

	cl::NDRange GlobalRange{vars_2};
	cl::NDRange LocalRange{(unsigned long long)group_size};
	cl::EnqueueArgs Args{*command_queue_, GlobalRange, LocalRange};

	qDebug() << "Variables cache size: " << sizeof(ES) * variables.size() * group_size;
	qDebug() << "Expression stack size: " << sizeof(ES) * (expression.size() - operCount) * group_size;


	cl::Event evt = (*process_vecs)(Args, vars_buf, variables.size(), expr_buf, expression.size(), operCount, trues_buf,
								 answer_buf, cl::Local(sizeof(ES) * (expression.size() - operCount) * group_size),
								 cl::Local(sizeof(ES) * variables.size() * group_size));
	result.resize(vars_2 * (variables.size() + operCount), true);
	evt.wait();

	cl::copy(*command_queue_, answer_buf, result.begin(), result.end());
	cl::copy(*command_queue_, trues_buf, &trues, (&trues) + 1);
}

unsigned int GPUComputeTable::power_of_2(unsigned int pow)
{
	unsigned long long result = 1;
	for (int i = 0; i < pow; i++)
	{
		result *= 2u;
	}
	return result;
}

unsigned int GPUComputeTable::align(unsigned int num)
{
	unsigned int result = 1;
	while (result * 2 < num)
	{
		result *= 2;
	}
	return result;
}
