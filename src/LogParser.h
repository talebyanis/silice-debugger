#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>

// Stores a line from the .v.vio.log report file :
// FILENAME - TOKEN - VARNAME - LINE - USAGE - INDEX
struct report_line {
	std::string filename;
	std::string token;
	std::string varname;
	std::string line;
	std::string usage;
	std::string index;
};

// Stores a line from the .v.index.log report file :
// INDEX - LINE_START - LINE_STOP - FILENAME
struct index_line {
	std::string index;
	std::pair<int, int> lines;
	std::string filename;
};


/*
LogParser :

Parses .v.vio.log and .v.index.log files generated after building a design
*/
class LogParser
{
public:
	LogParser();
	
	// Vio methods
	void parseVio(std::string vio_filename);
	std::string getCol(std::string file_name, std::string var_name, int col_nb);
	std::list<std::pair<std::string, std::string>> getMatch(std::string match);

	//Index methods
	void parseIndex(std::string index_filename);
	std::pair<int, int> getLines(std::string filename, std::string index);
	std::pair<std::string, std::string> getIndex(int line);
private:
	// (filename, varname) -> report_line
	std::map<std::pair<std::string, std::string>, report_line> report_lines;
	
	// (filename, index)   -> index_line
	std::map<std::pair<std::string, std::string>, index_line> index_lines;
};

