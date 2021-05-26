#include "LogParser.h"
#include <algorithm>

LogParser::LogParser() = default;

// Vio methods ---------------------------------------------------------

void LogParser::parseVio(std::string vio_filename)
{
    this->report_lines.clear();
	std::fstream file;

	file.open(vio_filename, std::ios::in);
	if (!file)
	{
		std::cout << "VIO Log file was not found";
		exit(1);
	}

	std::string element;
	report_line rl;
	while (file >> element) {
		rl.filename = element;
		file >> element;
		rl.token = element;
		file >> element;
		rl.varname = element;
		file >> element;
		rl.line = element;
		file >> element;
		rl.usage = element;

		report_lines.emplace(std::make_pair(std::make_pair(rl.filename, rl.varname), rl));
	}
}

// ---------------------------------------------------------------------

// Returns a specific column for a line
// col_nb : 1=filename, 2=token, 3=varname, 4=line, 5=usage
std::string LogParser::getCol(std::string file_name, std::string var_name, int col_nb)
{
	switch (col_nb)
	{
	case 0:
		return report_lines[std::make_pair(file_name, var_name)].filename;
		break;
	case 1:
		return report_lines[std::make_pair(file_name, var_name)].token;
		break;
	case 2:
		return report_lines[std::make_pair(file_name, var_name)].varname;
		break;
	case 3:
		return report_lines[std::make_pair(file_name, var_name)].line;
		break;
	case 4:
		return report_lines[std::make_pair(file_name, var_name)].usage;
		break;
	default:
		break;
	}
	std::cout << "LogParser::getCol, 0 <= col_nb <= 4" << std::endl;
	exit(1);
}

// ---------------------------------------------------------------------

// Returns a list of key (file_name, var_name) having their value's usage matching with the parameter (const, temp...)
std::list<std::pair<std::string, std::string>> LogParser::getMatch(const std::string& match)
{
	auto* list = new std::list<std::pair<std::string, std::string>>();
	for (auto const& [key, val] : report_lines)
	{
		if (val.usage == match)
		{
			list->push_back(key);
		}
	}
	return *list;
}

// FSM methods -------------------------------------------------------

void LogParser::parseFSM(const std::string& fsm_filename)
{
    this->fsm_lines.clear();
	std::fstream file;

	file.open(fsm_filename, std::ios::in);
	if (!file)
	{
		std::cout << "FSM Log file was not found";
		exit(1);
	}

	std::string element;
	fsm_line fsml;
	while (file >> element) {
		fsml.algo = element;
		file >> element;
		fsml.index = stoi(element);
		file >> element;
		fsml.filename = element;
		file >> element;
		fsml.line = stoi(element);

		fsm_lines.insert(fsm_lines.begin(), std::make_pair(std::make_pair(fsml.filename, fsml.index), fsml));
	}

	if (!fsm_lines.empty())
	{
		sort(fsm_lines.begin(), fsm_lines.end(), fsm_line::cmp);
	}

	// uncomment to print fsm_lines
	//for (const auto& i : this->fsm_lines)
	//{
	//	std::cout << i.second.filename << " " << i.second.line << " " << i.second.index << std::endl;
	//}
}

// ---------------------------------------------------------------------

// Return lines associated with "index" from "filename"
std::pair<int, int> LogParser::getLines(std::string filename, int index)
{
	std::pair<int, int> pair = std::make_pair(-1, -2); // -2 = inf
	bool found = false;
	for (auto const& [key, val] : this->fsm_lines)
	{
		if (found)
		{
            if (val.filename != filename)
                break;
			pair.second = val.line;
			return pair;
		}
		if (val.filename == filename && val.index == index)
		{
			pair.first = val.line;
			found = true;
		}
	}
	if (found)
	    pair.second = pair.first;

	return pair;
}

// ---------------------------------------------------------------------

std::list<int> LogParser::getIndexes(const std::string& filename)
{
    std::list<int> res;
    for (const auto &line : this->fsm_lines)
    {
        if (line.second.filename == filename)
        {
            res.push_back(line.second.index);
        }
    }
    return res;
}


// ---------------------------------------------------------------------

std::list<std::string> LogParser::getAlgos(const std::string& filename)
{
    std::list<std::string> res;
    bool found = false;
    for (const auto &line : this->fsm_lines)
    {
        if (filename == line.second.filename)
        {
            for (const auto &item : res)
            {
                if (item == line.second.algo)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                res.push_back(line.second.algo);
            }
            found = false;
        }
    }
    return res;
}