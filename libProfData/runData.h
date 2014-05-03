#ifndef __PROFILER_LIBPROFDATA_RUNDATA_H__
#define __PROFILER_LIBPROFDATA_RUNDATA_H__
#include "csv.h"
#include "util_time.h"
#include <string>

/*
 * This utility class is designed to read in a CSV of the form
 * "<TIMESTAMP>","[ENTER|LEAVE]","Funcition Name"
 *
 * Use:
 *   1: Construct the object with the filename of a valid data file.
 *   2: Loop over the events (0 to NumEvents()) and extract them with 
 *      GetEvent
 *   3: Do something useful with them...
 * 
 * If an a data row is invalid, or the index is out of bounds the returned
 * event will have a type of EVENT_TYPE_ERROR and event_name will contain the
 * error text.
 *
 */
class ProfDataFile: private CSV<Time,std::string,std::string>
{
public:
    struct Event {
        static const char EVENT_TYPE_ENTER = '>';
        static const char EVENT_TYPE_LEAVE = '<';
        static const char EVENT_TYPE_ERROR = '?';
        Time event_time;
        char event_type;
        std::string event_name;
    };
    ProfDataFile(const std::string& fname);

    int NumEvents() { 
        return Rows();
    }

    Event GetEvent(int i);
};
#endif
