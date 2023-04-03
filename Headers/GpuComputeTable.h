//
// Created by ivanl on 22.03.2023.
//

#ifndef DISCRETEMATH_HEADERS_GPUCOMPUTETABLE_H_
#define DISCRETEMATH_HEADERS_GPUCOMPUTETABLE_H_

#pragma once

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300
#define CL_HPP_MINIMUM_OPENCL_VERSION 300
#include <vector>
#include <CL/opencl.hpp>
#include <memory>
#include "Variable.h"
#include "Operation.h"

class GPUComputeTable
{
public:

	GPUComputeTable();

	void run(std::vector<bool> &result, const std::vector<Variable> &variables, const std::vector<ExpressionSymbol*> &expression,
			 const int operCount, unsigned int &trues);

	~GPUComputeTable();

private:

	struct ES
	{
		enum Type{
			Oper, Var
		};
		char Symbol;
		bool Value = 0;
		Type type;
	};

	unsigned int power_of_2(unsigned int pow);

	std::unique_ptr<cl::CommandQueue> command_queue_;

	std::unique_ptr<cl::Program> program_;

	std::unique_ptr<cl::Context> context_;

	int max_group_size;
};

#endif //DISCRETEMATH_HEADERS_GPUCOMPUTETABLE_H_
