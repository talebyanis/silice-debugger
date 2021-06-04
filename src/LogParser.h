#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <vector>
#include "sourcePath.h"
// Stores a line from the .v.vio.log report file :
// FILENAME - TOKEN - VARNAME - LINE - USAGE - VERILOG_NAME
struct report_line {
	std::string filename;
	std::string token;
	std::string varname;
	std::string line;
	std::string usage;
	std::string v_name;
};

// Stores a line from the .v.fsm.log report file :
// ALGO - INDEX - FILENAME - NUMBER OF INSTRUCTIONS - LINES...
struct fsm_line {
    /*
	// old structure
    std::string algo;
	int index;
	std::string filename;
	int line;
    */

    std::string algo;
    std::string filename;
    std::list<int> indexed_lines;

    /*
	// Comparator basing on lines
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
     */
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
	std::string getCol(const std::string& file_name, const std::string& var_name, int col_nb) const;
	std::list<std::pair<std::string, std::string>> getMatch(const std::string& match);
	report_line getLineFromVName(const std::string& match);

	// FSM methods
	std::list<int> getLines(const std::string& filename, int index, const std::string& algo);
    std::list<int> getIndexes(const std::string& filename);
	std::list<std::string> getAlgos(const std::string& filename);

private:
    void parseVio(const std::string& vio_filename);
	// (filename, varname) -> report_line
	std::map<std::pair<std::string, std::string>, report_line> report_lines;

    void parseFSM(const std::string& fsm_filename);
	// (filename, algoname, index) -> fsm_line
	std::map<std::pair<std::pair<std::string, std::string>, int>, fsm_line> fsm_lines;
};

