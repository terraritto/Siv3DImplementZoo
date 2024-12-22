#include "CubicFunction.h"

double CubicFunction::Calculate(const double x)
{
    return x * (x - 0.5) * (x + 0.5);
}
