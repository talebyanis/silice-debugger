#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>

// Stores a line from the report file :
// FILENAME - TOKEN - VARNAME - LINE - USAGE - INDEX - L_START - L_STOP
struct report_line {
	std::string filename;
	std::string token;
	std::string varname;
	std::string line;
	std::string usage;
	std::string index;
	int			line_start;
	int			line_stop;
};

/*
LogParser :

Parses the .v.vio.log file generated after building a design
*/
class LogParser
{
public:
	LogParser();
	LogParser(std::string report_filename);
	std::string getCol(std::string file_name, std::string var_name, int col_nb);
	std::list<std::pair<std::string, std::string>> getMatch(std::string match);
	std::pair<int, int> getLines(std::string file_name, std::string var_name);
private:
	// (filename, varname) -> report_line
	std::map<std::pair<std::string, std::string>, report_line> report_lines;
};

