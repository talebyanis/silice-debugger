#include "LogParser.h"

LogParser::LogParser() = default;

// ---------------------------------------------------------------------

LogParser::LogParser(std::string report_filename)
{
	std::fstream file;

	file.open(report_filename, std::ios::in);
	if (!file)
	{
		std::cout << "Log file was not found";
		exit(1);
	}

	std::string element;
	std::map<std::string, report_line> rls;
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

		auto truc = std::pair(rl.filename, rl.varname);
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

// ---------------------------------------------------------------------

// ???
std::pair<int, int> LogParser::getLines(std::string file_name, std::string var_name)
{
	for (auto const& [key, val] : this->report_lines)
	{
		if (val.filename == file_name && val.varname == var_name)
		{
			return std::pair<int, int>(val.line_start, val.line_stop);
		}
	}
	return std::pair(-1, -1);
}