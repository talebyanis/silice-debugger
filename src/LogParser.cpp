#include "LogParser.h"

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

		rls.insert(std::pair<std::string, report_line>(rl.varname, rl));
	}

	this->report_lines = rls;
}

// ---------------------------------------------------------------------

std::string LogParser::getCol(std::string var_name, int col_nb)
{
	switch (col_nb)
	{
	case 0:
		return this->report_lines[var_name].filename;
		break;
	case 1:
		return this->report_lines[var_name].token;
		break;
	case 2:
		return this->report_lines[var_name].varname;
		break;
	case 3:
		return this->report_lines[var_name].line;
		break;
	case 4:
		return this->report_lines[var_name].usage;
		break;
	default:
		break;
	}
	std::cout << "LogParser::getCol, 0 <= col_nb <= 4" << std::endl;
	exit(1);
}

// ---------------------------------------------------------------------