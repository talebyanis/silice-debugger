#include "LogParser.h"

std::map<std::string, report_line> LogParser::parse(std::string report_filename)
{
	std::fstream file;

	file.open(report_filename, std::ios::in);
	if (!file)
	{
		std::cout << "Log file was not found";
		return;
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
	
	return rls;
}

// ---------------------------------------------------------------------