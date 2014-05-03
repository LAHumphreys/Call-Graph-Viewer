#include "runData.h"
#include "stdReader.h"

#define CSV CSV<Time,std::string,std::string>
ProfDataFile::ProfDataFile(const std::string& fname) 
    : CSV(CSV::LoadCSV(
              BinaryReader(
                 IFStreamReader(fname.c_str())
                 )
              )
          )
{
}

ProfDataFile::Event ProfDataFile::GetEvent(int i) {
    Event e;
    if ( i < 0 || i >= NumEvents() ) {
        e.event_type = Event::EVENT_TYPE_ERROR;
        e.event_name = "Invalid Event Index";
    } else {
        std::string buf;
        CSV::GetRow(i,e.event_time, buf,e.event_name);
        if ( buf == "ENTER" ) {
            e.event_type = Event::EVENT_TYPE_ENTER;
        } else if (buf == "LEAVE")  {
            e.event_type = Event::EVENT_TYPE_LEAVE;
        } else {
            e.event_type = Event::EVENT_TYPE_ERROR;
            e.event_name = "Invalid Event Type";
        }
    }

    return e;
}
