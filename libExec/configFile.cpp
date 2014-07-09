#include "configFile.h"
#include "logger.h"
#include <boost/algorithm/string/trim.hpp>
#include <regex.h>

using namespace std;

CommandFile::CommandFile(const std::string& fname)
  : file(fname), ok (false)
{
    if (file.good()) {
        std::string command;
        command.reserve(1024);
        while ( GetNextCommand(command) ) {
            commands.push_back(command);
        }
        ok = true;
    } else {
       SLOG_FROM(LOG_WARNING,"CommandFile::CommandFile","Failed to load " << fname)
    }
}

bool CommandFile::GetNextCommand(std::string& buf) {
    bool found = false;
    buf.clear();
    /*
     * Keep searching until we run out of input
     * or we find a command
     */
    for( std::getline(file,buf); file.good() && !found;)
    { 
        /*
         * Not interested in leading / trailing whitespace...
         */
        boost::algorithm::trim(buf);
        found = LineIsCommand(buf);
        if ( !found ) {
            buf.clear();
             std::getline(file,buf);
        }
    }

    return found;

}

bool CommandFile::LineIsCommand(const std::string& line) {
    bool valid = false;

    
    if ( line != "" && !LineIsComment(line) ) {
        valid = true;
    }
    return valid;
}

bool CommandFile::LineIsComment(const std::string& line) {
    static RegPattern commentPattern("^ *//");
    return commentPattern.Search(line);
}

