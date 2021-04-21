#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>

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
	LogParser();
	LogParser(std::string report_filename);
	std::string getCol(std::string file_name, std::string var_name, int col_nb);
	std::list<std::pair<std::string, std::string>> LogParser::getMatch(std::string match);
private:
	// (filename, varname) -> report_line
	std::map<std::pair<std::string, std::string>, report_line> report_lines;
};

