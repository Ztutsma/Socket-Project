#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

std::vector<std::string> ParseMessage(std::string msg);
std::vector<std::string> ParseUInput(std::string input);
std::vector<std::string> FormatQuery(std::vector<std::string> args);
std::string FormatMessage(std::vector<std::string> args);