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

std::vector<std::string> FormatQuery(std::vector<std::string> args)
{
	std::string command = args[0];
	std::string query = "";

	int i = 1;
	while (i < args.size() - 1)
	{
		query += args[i] + " ";
		i++;
	}
	query += args[i];

	args.clear();
	args.push_back(command);
	args.push_back(query);

	return args;
}

// Converts list of arguments into a message
std::string FormatMessage(std::vector<std::string> args)
{
	std::string msg = "";

	for (int i = 0; i < args.size(); i++)
	{
		msg += args[i] + ";";
	}

	return msg;
}