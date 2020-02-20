#ifndef TEXT_H
#define TEXT_H


#include<string>
#include<vector>

namespace Text
{


inline std::vector<std::string> split(std::string phrase, std::string delimiter)
{

    std::vector<std::string> list;

    std::size_t pos = 0;
    std::string token;

    while ((pos = phrase.find(delimiter)) != std::string::npos)
    {
        token = phrase.substr(0, pos);
        list.push_back(token);
        phrase.erase(0, pos + delimiter.length());
    }

    list.push_back(phrase);

    return list;
}


}

#endif // TEXT_H
