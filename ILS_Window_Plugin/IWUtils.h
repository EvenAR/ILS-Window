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

COLORREF HexToRGB(const std::string& hexColor) {
    int red, green, blue;

    if (hexColor.size() == 7 && hexColor[0] == '#') {
        // Extract each color component from the hex string
        std::stringstream ss;
        ss << std::hex << hexColor.substr(1, 2); // Red
        ss >> red;
        ss.clear();

        ss << std::hex << hexColor.substr(3, 2); // Green
        ss >> green;
        ss.clear();

        ss << std::hex << hexColor.substr(5, 2); // Blue
        ss >> blue;
    }

    return RGB(red, green, blue);
}
