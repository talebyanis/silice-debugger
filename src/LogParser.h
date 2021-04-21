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
	LogParser(std::string report_filename);
	std::string getCol(std::string var_name, int col_nb);
private:
	std::map<std::string, report_line> report_lines;
};

