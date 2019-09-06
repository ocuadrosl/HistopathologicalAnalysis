#ifndef MATH_H
#define MATH_H

#include <iostream>
#include <cmath>
#include <typeinfo>

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

template< int orgMin, int  orgMax, int newMin, int newMax >
double minMax(double inputValue)
{
    double den = ( orgMax == orgMin) ?  0.00001 : static_cast<double>(orgMax - orgMin);
    return  static_cast<double>(((static_cast<double>(newMax - newMin) * static_cast<double>(inputValue - orgMin)) / den ) + static_cast<double>(newMin));
}

/*
 * Template function-object version
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
        return  static_cast<outputType>( ( ( (newMax - newMin) * ( static_cast<double>(inputValue) - orgMin) ) / den ) + newMin  );
    }

private:
    double orgMin;
    double orgMax;
    double newMin;
    double newMax;

};

/*

index version
*/
template<typename vectorT, unsigned size=3>
inline auto maxWithIndex(vectorT vector, unsigned &index)
{

    auto max = vector[0];
    index = 0;
    for (unsigned i=1 ; i<size ; ++i)
    {
        if(vector[i] > max)
        {
            max   = vector[index];
            index = i;
        }

    }

    return max;


}


template<typename vectorT, unsigned size=3>
inline auto minWithIndex(vectorT vector, unsigned &index)
{

    auto min = vector[0];
    index = 0;
    for (unsigned i=1 ; i<size ; ++i)
    {
        if(vector[index] < min)
        {
            min   = vector[index];
            index = i;
        }

    }

    return min;

}

template<typename vectorT, unsigned size=3>
inline auto max(vectorT vector)
{

    auto max = vector[0];
    for (unsigned index=1 ; index<size ; ++index)
    {
        if(vector[index] > max)
            max = vector[index];
        //max = (vector[i] > max) ? vector[i] : max;
    }
    return max;

}


template<typename vectorT, unsigned size=3>
inline auto min(vectorT vector)
{

    auto min = vector[0];
    for (unsigned index=1 ; index<size ; ++index)
    {
        if(vector[index] < min)
            min = vector[index];
        //min = (vector[i] < min) ? vector[i] : min;
    }
    return min;

}


/*
Octave version
*/
template <typename type = double>
inline type mod(type numer, type denom)
{
       return numer -  denom * std::floor(numer/denom);
}


}


#endif // MATH_H
