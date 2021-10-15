#pragma once

#include <iostream>
#include <vector>
#include <sstream>

static std::vector<std::string> ParseMessage(std::string msg);
static std::vector<std::string> ParseUInput(std::string input);
static std::string FormatMessage(std::vector<std::string> args);