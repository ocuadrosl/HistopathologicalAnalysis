#ifndef MATH_H
#define MATH_H

#include <iostream>

namespace Math
{

template<typename inputType = double, typename outputType = double>
inline outputType minMax(inputType inputValue, inputType orgMin, inputType orgMax, outputType newMin, outputType newMax)
{
    double den = ( orgMax == orgMin) ?  0.00001 : static_cast<double>(orgMax - orgMin);
    return  static_cast<outputType>(((static_cast<double>(newMax - newMin) * static_cast<double>(inputValue - orgMin)) / den ) + static_cast<double>(newMin));

}

/*
template args fast versions
double

*/

template< unsigned orgMin, unsigned  orgMax, unsigned newMin, unsigned newMax >
double minMax(double inputValue)
{
    double den = ( orgMax == orgMin) ?  0.00001 : static_cast<double>(orgMax - orgMin);
    return  static_cast<double>(((static_cast<double>(newMax - newMin) * static_cast<double>(inputValue - orgMin)) / den ) + static_cast<double>(newMin));
}

/*
 * Template function Object version
*/
template<typename inputType = double, typename outputType = double>
class MinMax
{

public:

    MinMax(inputType  orgMin, inputType  orgMax, outputType newMin, outputType newMax )
    {
        this->orgMin = orgMin;
        this->orgMax = orgMax;
        this->newMin = newMin;
        this->newMax = newMax;

    }

    inline outputType operator()(inputType inputValue)
    {
        double den = ( orgMax == orgMin) ?  static_cast<double>(orgMax - orgMin): 0.00001;
        return  static_cast<outputType>((( static_cast<double>(newMax - newMin) * static_cast<double>(inputValue - orgMin)) / den) + static_cast<double>(newMin));
    }

private:
    inputType  orgMin;
    inputType  orgMax;
    outputType newMin;
    outputType newMax;

};

}


#endif // MATH_H
