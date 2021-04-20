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
public:
	std::map<std::string, report_line> parse(std::string report_filename);
};

