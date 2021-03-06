//**********************************************************************
//
// Copyright (c) 2004
// Thomas Young, PathEngine
// Lyon, France
//
// All Rights Reserved
//
//**********************************************************************

#ifndef PLATFORM_COMMON_FLOAT_PERSISTENCE_INCLUDED
#define PLATFORM_COMMON_FLOAT_PERSISTENCE_INCLUDED

#include "common/STL/string.h"
#include "common/STL/sstream.h"
#include "common/CLib/stdlib.h"

inline float FloatFromPersistent(const char* ptr)
{
    if(*ptr == 'x')
    {
        unsigned long asUL = strtoul(ptr + 1, 0, 16);
        return *reinterpret_cast<float*>(&asUL);
    }
    double asDouble = strtod(ptr, 0);
    return static_cast<float>(asDouble);
}
inline std::string FloatToPersistent(const float value)
{
    {
        std::ostringstream oss;
        oss.precision(8);
        oss << value;
        if(FloatFromPersistent(oss.str().c_str()) == value)
        {
            return oss.str();
        }
    }
    const unsigned long* asUL = reinterpret_cast<const unsigned long*>(&value);
    std::ostringstream oss;
    oss.setf(std::ios_base::hex, std::ios_base::basefield);
    oss << 'x' << *asUL;
    return oss.str();
}

#endif
