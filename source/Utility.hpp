#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <Windows.h>
#include <fstream>
#include <sstream>

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

void error(std::string&& message)
{
    message.append("\n");
    OutputDebugString(message.c_str());    
    throw message;
}

std::string loadFile(std::string&& filePath)
{
    std::ifstream t(filePath);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

#endif;