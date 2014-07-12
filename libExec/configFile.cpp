#include "configFile.h"
#include "logger.h"
#include <boost/algorithm/string/trim.hpp>
#include <regex.h>

using namespace std;

CommandScript::CommandScript()
  : ok (false)
{
}

CommandScript::CommandScript(istream& input) 
   : ok (false)
{
    Initialise(input);
}

CommandScript::CommandScript(std::string script) 
   : ok (false)
{
    stringstream scriptStream(script);
    Initialise(scriptStream);
}

void CommandScript::Initialise(istream& input)
{
    std::string command;
    command.reserve(1024);
    while ( GetNextCommand(command, input) ) {
        commands.push_back(command);
    }
    ok = true;
}

CommandFile::CommandFile(const std::string& fname)
  : file(fname)
{
    if (file.good()) {
        Initialise(file);
    } else {
       SLOG_FROM(LOG_WARNING,"CommandScript::CommandScript","Failed to load " << fname)
    }
}

bool CommandScript::GetNextCommand(std::string& buf, istream& input) {
    bool found = false;
    buf.clear();
    /*
     * Keep searching until we run out of input
     * or we find a command
     */
    for( std::getline(input,buf); input.good() && !found;)
    { 
        /*
         * Not interested in leading / trailing whitespace...
         */
        boost::algorithm::trim(buf);
        found = LineIsCommand(buf);
        if ( !found ) {
            buf.clear();
             std::getline(input,buf);
        }
    }

    return found;

}

bool CommandScript::LineIsCommand(const std::string& line) {
    bool valid = false;

    
    if ( line != "" && !LineIsComment(line) ) {
        valid = true;
    }
    return valid;
}

bool CommandScript::LineIsComment(const std::string& line) {
    static RegPattern commentPattern("^ *//");
    return commentPattern.Search(line);
}

