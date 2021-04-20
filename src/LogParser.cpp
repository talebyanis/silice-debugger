#include "LogParser.h"


LogParser::LogParser(std::string report_filepath)
{
	this->filepath = report_filepath;
}

// ---------------------------------------------------------------------

void LogParser::parse()
{
	std::fstream file;

	file.open(this->filepath, std::ios::in);
	if (!file)
	{
		std::cout << "Log file was not found";
		return;
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

		this->report_lines.insert(std::pair<std::string, report_line>(rl.varname, rl));
	}
	
}