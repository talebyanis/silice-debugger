#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <vector>
// Stores a line from the .v.vio.log report file :
// FILENAME - TOKEN - VARNAME - LINE - USAGE
struct report_line {
	std::string filename;
	std::string token;
	std::string varname;
	std::string line;
	std::string usage;
};

// Stores a line from the .v.fsm.log report file :
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
			if (a.second.line == b.second.line)
			{
				return a.second.index < b.second.index;
			}
			else
			{
				return a.second.line < b.second.line;
			}
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
	void parseVio(const std::string& vio_filename);
	std::string getCol(const std::string& file_name, const std::string& var_name, int col_nb);
	std::list<std::pair<std::string, std::string>> getMatch(const std::string& match);

	// FSM methods
	void parseFSM(const std::string& fsm_filename);
	std::pair<int, int> getLines(const std::string& filename, int index);
    std::list<int> getIndexes(const std::string& filename);
	std::list<std::string> getAlgos(const std::string& filename);
private:
	// (filename, varname) -> report_line
	std::map<std::pair<std::string, std::string>, report_line> report_lines;
	
	// (filename, index)   -> fsm_line
	std::vector<std::pair<std::pair<std::string, int>, fsm_line>> fsm_lines;
};

