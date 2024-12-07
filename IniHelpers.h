#pragma once

#include <windows.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "ParDataTypes.h"

RGB HexToRGB(const std::string& hexColor) {
    RGB color = { 0, 0, 0 };

    if (hexColor.size() == 7 && hexColor[0] == '#') {
        // Extract each color component from the hex string
        std::stringstream ss;
        ss << std::hex << hexColor.substr(1, 2); // Red
        ss >> color.r;
        ss.clear();

        ss << std::hex << hexColor.substr(3, 2); // Green
        ss >> color.g;
        ss.clear();

        ss << std::hex << hexColor.substr(5, 2); // Blue
        ss >> color.b;
    }

    return color;
}

RGB ReadColorFromIni(const std::string& section, const std::string& key, const std::string& iniFilePath) {
    char colorStr[256] = { 0 };
    GetPrivateProfileStringA(section.c_str(), key.c_str(), "", colorStr, sizeof(colorStr), iniFilePath.c_str());

    std::string colorHex(colorStr);  // Convert to std::string
    return HexToRGB(colorHex);       // Convert hex to RGB
}