// Parser.h provides several methods used for formatting
// and parsing UDP messages
// 
// Name: Collin Amstutz
// Student ID: 1208893539
// Class: CSE 434
//

#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

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

	// Get first argument
	std::getline(stream, segment, ' ');
	// Convert string to lowercase
	std::transform(segment.begin(), segment.end(), segment.begin(), [](unsigned char c) -> unsigned char { return std::tolower(c); });
	args.push_back(segment);

	// Queries are case sensitive
	// Do not convert arguments to lowercase
	if (args[0] == "query")
	{
		while (std::getline(stream, segment, ' '))
		{
			args.push_back(segment);
		}
		return args;
	}

	while (std::getline(stream, segment, ' '))
	{
		// Convert strings to lowercase
		std::transform(segment.begin(), segment.end(), segment.begin(), [](unsigned char c) -> unsigned char { return std::tolower(c); });
		args.push_back(segment);
	}

	return args;
}

// Converts all arguments of a "query" into one space-separated string
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