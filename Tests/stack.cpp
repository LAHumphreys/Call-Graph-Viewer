#include "tester.h"
#include "stack.h"
#include "util_time.h"

using namespace std;

int Basic(testLogger& log);
int InvalidFrame(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Executing a valid stack...",Basic).RunTest();
    Test("Executing a stack with an invalid frame...",InvalidFrame).RunTest();
    return 0;
}

int Basic(testLogger& log) {
    long mainTime, basicTime;
    CallStack stack;
    stack.AddFrame("main",Time("20140703 10:11:02.194930"));
    stack.AddFrame("Basic",Time("20140703 10:11:02.194931"));

    if ( !stack.LeaveFrame("Basic",
                           Time("20140703 10:11:02.194932"),
                           basicTime) )
    {
        log << "Failed to leave frame Basic!" << endl;
        return 1;
    }
    if ( basicTime != 1 ) {
        log << "Invalid basic time! " << basicTime << endl;
        return 1;
    }

    if ( !stack.LeaveFrame("main",
                           Time("20140703 10:11:03.194931"),
                           mainTime) )
    {
        log << "Failed to leave frame main!" << endl;
        return 1;
    }

    if ( mainTime != 1000001 ) {
        log << "Invalid basic time! " << mainTime << endl;
        return 1;
    }

    return 0;
}

int InvalidFrame(testLogger& log) {
    long mainTime, basicTime, rogueTime;
    CallStack stack;
    stack.AddFrame("main",Time("20140703 10:11:02.194930"));
    stack.AddFrame("Basic",Time("20140703 10:11:02.194931"));

    if ( stack.LeaveFrame("Rogue",
                           Time("20140703 10:11:02.194932"),
                           rogueTime) )
    {
        log << "Left a rogue stack frame!" << endl;
        return 1;
    }

    if ( rogueTime != -1 ) {
        log << "Invalid rogue time! " << rogueTime << endl;
        return 1;
    }

    if ( !stack.LeaveFrame("Basic",
                           Time("20140703 10:11:02.194932"),
                           basicTime) )
    {
        log << "Failed to leave frame Basic!" << endl;
        return 1;
    }
    if ( basicTime != 1 ) {
        log << "Invalid basic time! " << basicTime << endl;
        return 1;
    }

    if ( !stack.LeaveFrame("main",
                           Time("20140703 10:11:03.194931"),
                           mainTime) )
    {
        log << "Failed to leave frame main!" << endl;
        return 1;
    }

    if ( mainTime != 1000001 ) {
        log << "Invalid basic time! " << mainTime << endl;
        return 1;
    }

    return 0;
}
