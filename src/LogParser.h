#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <vector>


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
// ALGO - INDEX - FILENAME - LINE (starting line)
struct fsm_line {
	std::string algo;
	int index;
	std::string filename;
	int line;

	// Comparator used in sortFSMMap() basing on lines
	static bool cmp(std::pair<std::pair<std::string, int>, fsm_line>& a, std::pair<std::pair<std::string, int>, fsm_line>& b)
	{
		// same filename
		if (a.first.first == b.first.first)
		{
			// comparing lines
			return a.second.line < b.second.line;
		}
		return a.first.first < b.first.first;
	}
};


/*
LogParser :

Parses .v.vio.log and .v.fsm.log files generated after building a design
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
	void parseFSM(std::string fsm_filename);
	std::pair<int, int> getLines(std::string filename, int index);
	std::pair<std::string, int> getIndex(int line);
private:
	// (filename, varname) -> report_line
	std::map<std::pair<std::string, std::string>, report_line> report_lines;
	
	// (filename, index)   -> fsm_line
	std::vector<std::pair<std::pair<std::string, int>, fsm_line>> fsm_lines;
};

