#include "Parser.h"

// Converts message into a list of arguments
std::vector<std::string> ParseMessage(std::string msg)
{
	std::vector<std::string> args;
	std::string segment;
	std::stringstream stream(msg);

	while (std::getline(stream, segment, ';'))
	{
		args.push_back(segment);
	}

	return args;
}

// Converts user input into a list of arguments
std::vector<std::string> ParseUInput(std::string input)
{
	std::vector<std::string> args;
	std::string segment;
	std::stringstream stream(input);

	while (std::getline(stream, segment, ' '))
	{
		// Convert strings to lowercase
		std::transform(segment.begin(), segment.end(), segment.begin(), [](unsigned char c) -> unsigned char { return std::tolower(c); });
		args.push_back(segment);
	}

	return args;
}

// Converts list of arguments into a message
std::string FormatMessage(std::vector<std::string> args)
{
	std::string msg = "";

	for (int i = 0; i < args.size(); i++)
	{
		msg += args[1] + ";";
	}

	return msg;
}