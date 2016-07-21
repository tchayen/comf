#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <Windows.h>
#include <fstream>
#include <sstream>

void log(std::string&& message)
{
    message.append("\n");
    OutputDebugString(message.c_str());    
    std::runtime_error(message.c_str());
}

std::string loadFile(std::string&& filePath)
{
    std::ifstream t(filePath);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

#endif;