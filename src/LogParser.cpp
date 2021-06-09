#include "LogParser.h"
#include <algorithm>

LogParser::LogParser()
{
    this->parseAlgo(PROJECT_DIR "BUILD_icarus/build.v.alg.log");
    this->parseVio(PROJECT_DIR "BUILD_icarus/build.v.vio.log");
    this->parseFSM(PROJECT_DIR "BUILD_icarus/build.v.fsm.log");
}

// Algo methods --------------------------------------------------------

// instance - algo - d_name - path
void LogParser::parseAlgo(const std::string& algo_filename)
{
    this->algo_lines.clear();
    std::fstream file;
    file.open(algo_filename, std::ios::in);
    if (!file)
    {
        std::cout << "Algo Log file was not found";
        exit(1);
    }

    std::string element;
    algo_line al;
    while (file >> element) {
        // instance
        al.instance = element;
        file >> element;

        // algo
        al.algo = element;
        file >> element;

        // d_name
        al.d_name = element;
        file >> element;

        // path
        al.path = element;

        this->algo_lines.emplace(std::make_pair(al.instance, al));
    }
    // uncomment to print algo_lines
    /*
    for (const auto& i : this->algo_lines)
    {
        std::cout << i.second.instance << " " << i.second.algo << " " << i.second.path << " " << i.second.d_name << std::endl;
    }
    */
}

// Vio methods ---------------------------------------------------------

void LogParser::parseVio(const std::string& vio_filename)
{
    this->report_lines.clear();
    // Looks for every Silice files needed in the design
    std::ifstream file(vio_filename);

    report_line rl;
    std::string instance;
    int nb_line;
    if (file.is_open())
    {
        while (file.good())
        {
            file >> instance;
            file >> nb_line;
            for (int i = 0; i < nb_line; ++i) {
                file >> rl.token;
                file >> rl.varname;
                file >> rl.line;
                file >> rl.type;
                file >> rl.usage;
                file >> rl.v_name;
                rl.filename = this->algo_lines[instance].path;
                this->report_lines[std::make_pair(rl.filename, rl.varname)] = rl;
            }
        }
        file.close();
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

void LogParser::parseFSM(const std::string& fsm_filename)
{
    this->fsm_lines.clear();
    // Looks for every Silice files needed in the design
    std::ifstream file(fsm_filename);

    fsm_line fsml;
    std::string instance;
    int index, nb_line, line_number;
    if (file.is_open())
    {
        while (file.good())
        {
            file >> instance;
            file >> index;
            file >> nb_line;
            for (int i = 0; i < nb_line; ++i) {
                file >> line_number;
                fsml.indexed_lines.push_back(line_number);
            }
            if (nb_line > 0)
            {
                fsml.algo = this->algo_lines[instance].algo;
                fsml.filename = this->algo_lines[instance].path;
                this->fsm_lines[std::make_pair(std::make_pair(fsml.filename, fsml.algo), index)] = fsml;
            }
        }
        file.close();
    }
    // uncomment to print report_lines
    /*
    for (const auto& i : this->fsm_lines)
    {
        std::cout << "1. " << i.second.filename << "\n2. " << i.second.algo << "\n3. " << i.second.indexed_lines.size() << std::endl;
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
        if (item.second.v_name.find(',') != std::string::npos && item.second.v_name.size() == 2 * match.size() + 1) {
            if (item.second.v_name.substr(0, match.size()) == match
                || item.second.v_name.substr(match.size() + 1, match.size()) == match) {
                return item.second;
            }
        } else if (item.second.v_name == match) {
            return item.second;
        }
    }
    return rl;
}

algo_line LogParser::getAlgoLine(std::string name)
{
    algo_line al;
    for (const auto &item : algo_lines) {
        if(item.second.algo == name) {
            return item.second;
        }
    }
    return al;
}