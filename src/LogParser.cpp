#include "LogParser.h"
#include <algorithm>

LogParser::LogParser()
{
    this->parseVio(PROJECT_DIR "BUILD_icarus/build.v.vio.log");
    this->parseFSM(PROJECT_DIR "BUILD_icarus/build.v.fsm.log");
}

// Vio methods ---------------------------------------------------------

void LogParser::parseVio(const std::string& vio_filename)
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
		//file >> element;
        //rl.v_name = element;

		report_lines.emplace(std::make_pair(std::make_pair(rl.filename, rl.varname), rl));
	}
}

// ---------------------------------------------------------------------

// Returns a specific column for a line
// col_nb : 0=filename, 1=token, 2=varname, 3=line, 4=usage, 5=v_name
std::string LogParser::getCol(const std::string& file_name, const std::string& var_name, int col_nb) const
{
    // Looking for the key (filename, varname)
    // If not found, returning "#" (= none)
    auto pair = std::make_pair(file_name, var_name);
    if (this->report_lines.find(pair) == this->report_lines.end())
    {
        return "#";
    }

	switch (col_nb)
	{
	case 0:
		return report_lines.at(pair).filename;
	case 1:
		return report_lines.at(pair).token;
	case 2:
		return report_lines.at(pair).varname;
	case 3:
		return report_lines.at(pair).line;
	case 4:
		return report_lines.at(pair).usage;
    case 5:
        return report_lines.at(pair).v_name;
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
std::pair<int, int> LogParser::getLines(const std::string& filename, int index)
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

// ---------------------------------------------------------------------

report_line LogParser::getLineFromVName(const std::string& match)
{
    report_line rl;
    rl.v_name = "#";
    for (const auto &item : this->report_lines)
    {
        if (item.second.v_name == match)
        {
            return item.second;
        }
    }
    return rl;
}