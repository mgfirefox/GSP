#pragma once
#include <string>

#include "IntUtility.h"

inline std::string getFileFormat(std::string filename)
{
    uint64 index = filename.find_last_of('.');
    if (index >= filename.size() - 1)
    {
        return "";
    }

    return filename.substr(index + 1);
}
