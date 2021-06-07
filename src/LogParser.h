#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <vector>
#include "sourcePath.h"

// Stores a line from the .v.alg.log report file :
// INSTANCE - ALGO - DISPLAY NAME - PATH
struct algo_line
{
    std::string instance;
    std::string algo;
    std::string d_name;
    std::string path;
};

// Stores a line from the .v.vio.log report file :
// FILENAME - TOKEN - VARNAME - LINE - USAGE - VERILOG_NAME
struct report_line
{
	std::string filename;
	std::string token;
	std::string varname;
	std::string type;
	std::string line;
	std::string usage;
	std::string v_name;
};

// Stores a line from the .v.fsm.log report file :
// ALGO - INDEX - FILENAME - NUMBER OF INSTRUCTIONS - LINES...
struct fsm_line
{
    std::string algo;
    std::string filename;
    std::list<int> indexed_lines;
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
	[[nodiscard]] std::string getCol(const std::string& file_name, const std::string& var_name, int col_nb) const;
	std::list<std::pair<std::string, std::string>> getMatch(const std::string& match);
	report_line getLineFromVName(const std::string& match);

	// FSM methods
	std::list<int> getLines(const std::string& filename, int index, const std::string& algo);
    std::map<int, std::list<std::string>> getIndexes(const std::string& filename);
	std::list<std::string> getAlgos(const std::string& filename);
	algo_line getAlgoLine(std::string name);

private:
    void parseAlgo(const std::string& algo_filename);
    // instance -> algo_line
    std::map<std::string, algo_line> algo_lines;

    void parseVio(const std::string& vio_filename);
	// (filename, varname) -> report_line
	std::map<std::pair<std::string, std::string>, report_line> report_lines;

    void parseFSM(const std::string& fsm_filename);
	// (filename, algoname, index) -> fsm_line
	std::map<std::pair<std::pair<std::string, std::string>, int>, fsm_line> fsm_lines;
};

