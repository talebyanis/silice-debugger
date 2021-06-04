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
	    // filename
		rl.filename = element;
		file >> element;

		// token / group
		rl.token = element;
		file >> element;

		// varname
		rl.varname = element;
		file >> element;

		// line
		rl.line = element;
		file >> element;

		// usage
		rl.usage = element;

		// v_name
		file >> element;
        rl.v_name = element;

		report_lines.emplace(std::make_pair(std::make_pair(rl.filename, rl.varname), rl));
	}

    // uncomment to print report_lines
    /*
    for (const auto& i : this->report_lines)
    {
        std::cout << i.second.filename << " " << i.second.varname << " " << i.second.usage << std::endl;
    }
    */
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

void LogParser::parseFSM(const std::string& fsm_filename) {
    this->fsm_lines.clear();
    std::fstream file;

    file.open(fsm_filename, std::ios::in);
    if (!file) {
        std::cout << "FSM Log file was not found";
        exit(1);
    }

    std::string element;
    int index, nblines;
    int line1, line2;
    std::list<int> lines;
    fsm_line fsml;
    while (file >> element) {
        // algo
        fsml.algo = element;
        file >> element;

        // index
        index = stoi(element);
        file >> element;

        // filename
        fsml.filename = element;
        file >> element;

        // nb lines
        nblines = stoi(element);

        // lines...
        lines.clear();
        for (int i = 0; i < nblines; ++i) {
            file >> element;
            size_t delimiter;
            if ((delimiter = element.find(',')) != std::string::npos) {
                line1 = stoi(element.substr(0, delimiter));
                line2 = stoi(element.substr(1, delimiter));
                for (int j = line1; j <= line2; ++j) {
                    lines.push_back(j);
                }
            } else {
                lines.push_back(stoi(element));
            }
            fsml.indexed_lines = lines;
        }
        fsm_lines.insert(std::make_pair(std::make_pair(std::make_pair(fsml.filename, fsml.algo), index), fsml));
    }

    // uncomment to print fsm_lines
    /*
    for (const auto &i : this->fsm_lines) {
        std::cout << i.second.filename << " " << i.second.algo << " " << i.first.second << std::endl;
    }
    */
}

// ---------------------------------------------------------------------

std::list<int> LogParser::getLines(const std::string& filename, int index, const std::string& algo)
{
	return this->fsm_lines[std::make_pair(std::make_pair(filename, algo), index)].indexed_lines;
}

// ---------------------------------------------------------------------

std::map<int, std::list<std::string>> LogParser::getIndexes(const std::string& filename)
{
    std::map<int, std::list<std::string>> res;
    for (const auto &[key, fsmline] : this->fsm_lines)
    {
        if (fsmline.filename == filename)
        {
            for (const auto &line : fsmline.indexed_lines)
            {
                if (res.find(line) == res.end())
                {
                    res[line].push_back(fsmline.algo);
                }
                else
                {
                    if (std::find(res[line].begin(), res[line].end(), fsmline.algo) == res[line].end())
                    {
                        res[line].push_back(fsmline.algo);
                    }
                }

            }
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
    for (const auto &item : this->report_lines) {
        if (item.second.v_name == match ||
            (item.second.v_name.find(',') != std::string::npos && (item.second.v_name.find(match) == 0 || item.second.v_name.find(match) == match.size()+1))
            ) {
            return item.second;
        }
    }
    return rl;
}