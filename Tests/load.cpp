#include "tester.h"
#include "runData.h"
#include <vector>

using namespace std;

int Load(testLogger& log );

int main(int argc, const char *argv[])
{
    Test("Reading test file...",Load).RunTest(); 
    return 0;
}

int Load(testLogger& log ) {
    /*
     *
     * Events: 
     *  0: "20140703 10:11:02.194930","ENTER","main(int argc,char** argv)"
     *  1: "20140703 10:11:02.194955","ENTER","Test()"
     *  2: "20140703 10:11:02.394930","LEAVE","Test()"
     *  3: "20140703 10:11:02.394930","LEAVE","main(int argc,char** argv)"
     *  4: "20140703 10:11:02.394930","ERROR","main(int argc,char** argv)"
     */
    ProfDataFile data("data/basicInput.csv");
    vector<string> times({
         "20140703 10:11:02.194930",
         "20140703 10:11:02.194955",
         "20140703 10:11:02.394930",
         "20140703 10:11:02.394930",
         "20140703 10:11:02.394930"
    });
    vector<char> types({
        ProfDataFile::Event::EVENT_TYPE_ENTER,
        ProfDataFile::Event::EVENT_TYPE_ENTER,
        ProfDataFile::Event::EVENT_TYPE_LEAVE,
        ProfDataFile::Event::EVENT_TYPE_LEAVE,
        ProfDataFile::Event::EVENT_TYPE_ERROR
    });
    vector<string> names({
         "main(int argc,char** argv)",
         "Test()",
         "Test()",
         "main(int argc,char** argv)",
         "Invalid Event Type"
    });

    if (data.NumEvents() != 5 ) {
        log << "Expected 5 events, but got: " << data.NumEvents();
        return 1;
    }

    for ( int i=0; i<data.NumEvents(); ++i ) {
        log << "Checking event " << i << endl;

        auto e = data.GetEvent(i);
        if ( e.event_type != types[i] ) {
            log << "Invalid event type!" << endl;
            log << "Expected: " << types[i] << endl;
            log << "Actual:   " << e.event_type << endl;
            return 1;
        }
        if ( e.event_time.Timestamp() != times[i] ) {
            log << "Invalid event time!" << endl;
            log << "Expected: " << times[i] << endl;
            log << "Actual:   " << e.event_time.Timestamp() << endl;
            return 1;
        }
        if ( e.event_name != names[i] ) {
            log << "Invalid event name!" << endl;
            log << "Expected: " << names[i] << endl;
            log << "Actual:   " << e.event_name << endl;
            return 1;
        }
    }
    return 0;
}
