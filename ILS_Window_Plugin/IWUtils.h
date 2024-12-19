#pragma once

#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "IWDataTypes.h"
#include <regex>

std::string trimString(const std::string& value) {
    return std::regex_replace(value, std::regex("^ +| +$|( ) +"), "$1");
}

std::string stringToUpper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::toupper(c); } // correct
    );
    return s;
}

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
