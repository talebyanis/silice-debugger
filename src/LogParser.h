#include <string>
#include <iostream>
#include <fstream>
#include <map>

struct report_line {
	std::string filename;
	std::string token;
	std::string varname;
	std::string line;
	std::string usage;
};

class LogParser
{
	std::string filepath;
	std::map<std::string, report_line> report_lines;
public:
	LogParser(std::string report_filename);
	void parse();
};

