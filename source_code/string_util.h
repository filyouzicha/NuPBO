#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <string>
#include <limits>

class StringUtil {
    public:
    // string to unsigned int，uint32、uint64、uint16、uint8
    template <class T>
    static bool String2Uint(const std::string& str, T& dst);
};

template <class T>
bool StringUtil::String2Uint(const std::string& str, T& dst)
{
    if(str.empty() || str[0] == '-')
    {
        return false;
    }

    char* check;
    errno = 0;
    unsigned long long value = strtoull(str.c_str(), &check, 0);
    if(errno == ERANGE || *check || value > std::numeric_limits<T>::max())
    {
        return false;
    }

    dst = (T)value;
    return true;
}

#endif