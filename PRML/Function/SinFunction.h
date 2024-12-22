#pragma once
#include "../FunctionBase.h"

class SinFunction : public FunctionBase
{
public:
	using FunctionBase::FunctionBase;

protected:
	virtual double Calculate(const double x) final;
};
