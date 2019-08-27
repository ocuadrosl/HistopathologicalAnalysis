#ifndef MATH_H
#define MATH_H

#include <iostream>
#include <cmath>

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
        this->orgMin = static_cast<double>(orgMin);
        this->orgMax = static_cast<double>(orgMax);
        this->newMin = static_cast<double>(newMin);
        this->newMax = static_cast<double>(newMax);

   }

    inline outputType operator()(inputType inputValue)
    {
        double den = orgMax - orgMin;


        double rounded = std::round(( ( (newMax - newMin) * ( static_cast<double>(inputValue) - orgMin) ) / den ) + newMin);

        //std::cout<< rounded <<std::endl;
        return  static_cast<outputType>( ( ( (newMax - newMin) * ( static_cast<double>(inputValue) - orgMin) ) / den ) + newMin  );
    }

private:
    double orgMin;
    double orgMax;
    double newMin;
    double newMax;

};

}


#endif // MATH_H
