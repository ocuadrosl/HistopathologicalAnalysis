#ifndef MATH_H
#define MATH_H

#include <iostream>
#include <cmath>
#include <typeinfo>
#include <vector>
#include <algorithm>

namespace Math
{





template<typename type>
inline void diff(const std::vector<type>& data, std::vector<type>& output)
{

    for(unsigned i=1; i < data.size(); ++i)
    {
        output.push_back(data[i] - data[i-1]);

    }

}


template<typename type>
inline void vectorProduct(const std::vector<type>& a, const std::vector<type>& b,  std::vector<type>& output)
{

    output = std::vector<type>(a.size());

    for(unsigned i=0; i < a.size(); ++i)
    {
        output[i] = a[i] * b[i];
    }

}




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


template <typename vectorT, unsigned size=3, typename outputT=float>
inline outputT squaredEuclideanDistance( vectorT vector1, vectorT vector2)
{

    outputT sum=0;
    for(unsigned i=0 ; i < size; ++i)
    {
        sum += std::pow(static_cast<outputT>(vector1[i]) - static_cast<outputT>(vector2[i]), 2.0);
    }
    return sum;


}



template <typename vectorT, unsigned size=3, typename outputT=float>
inline outputT euclideanDistance( vectorT vector1, vectorT vector2)
{
    return std::sqrt(squaredEuclideanDistance<vectorT, size, outputT>(vector1, vector2));
}




template<typename type=double>
inline double gaussian(type x, double s, double m=0) //s= sigma, m = mean
{

    const double pi= 3.14159;
    return (1.0 /(s * std::sqrt(2*pi))) * std::exp(-0.5 * std::pow((x-m)/s, 2));

}




inline double gaussian(double x, double y, double s, double mx=0, double my=0) //s= sigma, m = mean
{

    const double pi= 3.14159;

    double s2 = s*s;

    double firstTerm = 1.0 / (2*pi*s2);

    double expTerm =  std::exp(-((std::pow(x-mx,2)+std::pow(y-my,2))/(2*s2)));

    return firstTerm*expTerm;


}


template<typename type=float>
inline type LoG(type x, type y, type s) //s = sigma
{
    const float pi = 3.14159f;

    type s2 = s*s;
    type x2 = x*x;
    type y2 = y*y;

    type firstTerm  = static_cast<type>(-1.0/(pi*std::pow(s,4)));
    type secondTerm = static_cast<type>(1.0 - ( (x2+y2)/(2*s2)));
    type expTerm    = static_cast<type>(std::exp(- ((x2+y2)/(2*s2))));

    return firstTerm*secondTerm*expTerm;


}




}


#endif // MATH_H
