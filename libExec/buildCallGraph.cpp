#include "buildCallGraph.h"
#include "stack.h"
#include "runData.h"

void CallProfile::ProcessFile(const std::string& datafile)
{
    ProfDataFile data(datafile);

    // ProfDataFile has parsed the input, now replay the program...
    CallStack stack(&callGraph);
    for ( int i=0; i<data.NumEvents(); ++i ) {
        auto e = data.GetEvent(i);

        switch ( e.event_type ) {
        case ProfDataFile::Event::EVENT_TYPE_ENTER:
             SLOG_FROM(LOG_VERY_VERBOSE, "CallProfile::CallProfile",
                "Entered function " << e.event_name << " at " << e.event_time.Timestamp())

             stack.AddFrame(e.event_name, e.event_time);
             break;
        case ProfDataFile::Event::EVENT_TYPE_LEAVE:
             long usecs;

             if ( !stack.LeaveFrame(e.event_name,e.event_time,usecs) ) {
                 SLOG_FROM(LOG_ERROR, "CallProfile::CallProfile",
                    "Failed to leave function " << e.event_name << " at " << e.event_time.Timestamp();)
             } else {
                 SLOG_FROM(LOG_VERY_VERBOSE, "CallProfile::CallProfile",
                    "Left function " << e.event_name << " at " << e.event_time.Timestamp())
                 counter.AddCall(e.event_name,usecs);
             }
             break;
        case ProfDataFile::Event::EVENT_TYPE_ERROR:
        default:
             // There's nothing we can do with this...
             SLOG_FROM(LOG_WARNING, "CallProfile::CallProfile",
                "Discarding invalid event because " << e.event_name)
             break;
        }
    }
}
