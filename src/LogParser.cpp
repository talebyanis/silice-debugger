#include "LogParser.h"

LogParser::LogParser() = default;

// Vio methods ---------------------------------------------------------

void LogParser::parseVio(std::string vio_filename)
{
	std::fstream file;

	file.open(vio_filename, std::ios::in);
	if (!file)
	{
		std::cout << "Log file was not found";
		exit(1);
	}

	std::string element;
	report_line rl;
	while (file >> element) {
		rl.filename = element;
		file >> element;
		rl.token = element;
		file >> element;
		rl.varname = element;
		file >> element;
		rl.line = element;
		file >> element;
		rl.usage = element;

		this->report_lines.emplace(std::make_pair(std::make_pair(rl.filename, rl.varname), rl));
	}
}

// ---------------------------------------------------------------------

// Returns a specific column for a line
// col_nb : 1=filename, 2=token, 3=varname, 4=line, 5=usage
std::string LogParser::getCol(std::string file_name, std::string var_name, int col_nb)
{
	switch (col_nb)
	{
	case 0:
		return this->report_lines[std::make_pair(file_name, var_name)].filename;
		break;
	case 1:
		return this->report_lines[std::make_pair(file_name, var_name)].token;
		break;
	case 2:
		return this->report_lines[std::make_pair(file_name, var_name)].varname;
		break;
	case 3:
		return this->report_lines[std::make_pair(file_name, var_name)].line;
		break;
	case 4:
		return this->report_lines[std::make_pair(file_name, var_name)].usage;
		break;
	default:
		break;
	}
	std::cout << "LogParser::getCol, 0 <= col_nb <= 4" << std::endl;
	exit(1);
}

// ---------------------------------------------------------------------

// Returns a list of key (file_name, var_name) having their value's usage matching with the parameter (const, temp...)
std::list<std::pair<std::string, std::string>> LogParser::getMatch(std::string match)
{
	std::list<std::pair<std::string, std::string>> list;
	for (auto const& [key, val] : this->report_lines)
	{
		if (val.usage == match)
		{
			list.push_back(key);
		}
	}
	return list;
}

// Index methods -------------------------------------------------------

void LogParser::parseIndex(std::string index_filename)
{
	std::fstream file;

	file.open(index_filename, std::ios::in);
	if (!file)
	{
		std::cout << "Log file was not found";
		exit(1);
	}

	std::string element;
	index_line il;
	std::pair<int, int> lines;
	while (file >> element) {
		il.index = element;
		file >> element;
		lines.first = stoi(element);
		file >> element;
		lines.second = stoi(element);;
		file >> element;
		il.filename = element;

		this->index_lines.emplace(std::make_pair(std::make_pair(il.filename, il.index), il));
	}
}

// ---------------------------------------------------------------------

// Return lines associated with "index" from "filename"
std::pair<int, int> LogParser::getLines(std::string filename, std::string index)
{
	std::pair<int, int> pair = std::make_pair(-1, -1);
	for (auto const& [key, val] : this->index_lines)
	{
		if (val.filename == filename && val.index == index)
		{
			return val.lines;
		}
	}
	return pair;
}

// ---------------------------------------------------------------------

// Return the index which lines includes "line"
std::pair<std::string, std::string> LogParser::getIndex(int line)
{
	std::pair<std::string, std::string> pair = std::make_pair("", "");
	for (auto const& [key, val] : this->index_lines)
	{
		if (val.lines.first <= line && val.lines.second >= line)
		{
			return key;
		}
	}
	return pair;
}