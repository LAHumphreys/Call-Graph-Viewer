#include "tester.h"
#include "stack.h"
#include "util_time.h"
#include "runData.h"

using namespace std;

int Basic(testLogger& log);
int InvalidFrame(testLogger& log);
int LongUnwind(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Executing a valid stack...",Basic).RunTest();
    Test("Executing a stack with a long unwind...",LongUnwind).RunTest();
    Test("Executing a stack with an invalid frame...",InvalidFrame).RunTest();
    return 0;
}

int Basic(testLogger& log) {
    long mainTime, basicTime;
    Node n;
    CallStack stack(&n);
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

int LongUnwind(testLogger& log) {
    /*
     * _global (0.200021)
     *    |
     *    main (0.200000)
     *       |
     *       Test (0.199975)
     *          |
     *          Test2 (0.199954)
     *    |
     *    _final (0.000020)
     */
    Node n;
    CallStack stack(&n);
    ProfDataFile data("data/longUnwind.csv");
    for ( int i=0; i<data.NumEvents(); ++i ) {
        ProfDataFile::Event e = data.GetEvent(i);
        if (e.event_type == ProfDataFile::Event::EVENT_TYPE_ENTER) {
            stack.AddFrame(e.event_name,e.event_time);
        } else {
            long us;
            if (!stack.LeaveFrame(e.event_name,e.event_time,us,true)) {
                log << "Failed to leave frame: " << e.event_name;
                return 1;
            }
        }
    }

    NodePtr _global = n.GetNode(Path("_global"));
    NodePtr main = n.GetNode(Path("_global/main"));
    NodePtr test = n.GetNode(Path("_global/main/Test"));
    NodePtr test2 = n.GetNode(Path("_global/main/Test/Test2"));
    NodePtr final = n.GetNode(Path("_global/_final"));

    log << endl << "Children: " << n.NumChildren() << endl;

    if (_global.IsNull()) {
        log << "could not find _global!";
        return 1;
    }

    if (_global->RunTime() != 200021) {
        log << "Invalid _global runtime: " << _global->RunTime() << endl;
        return 1;
    }

    if (main.IsNull()) {
        log << "could not find main!";
        return 1;
    }

    if (main->RunTime() != 200000) {
        log << "Invalid main runtime: " << main->RunTime() << endl;
        return 1;
    }

    if (test.IsNull()) {
        log << "could not find test!";
        return 1;
    }

    if (test->RunTime() != 199975) {
        log << "Invalid test runtime: " << test->RunTime() << endl;
        return 1;
    }

    if (test2.IsNull()) {
        log << "could not find test2!";
        return 1;
    }

    if (test2->RunTime() != 199954) {
        log << "Invalid test2 runtime: " << test2->RunTime() << endl;
        return 1;
    }

    if (final.IsNull()) {
        log << "could not find final!";
        return 1;
    }

    if (final->RunTime() != 20) {
        log << "Invalid final runtime: " << final->RunTime() << endl;
        return 1;
    }

    return 0;
}
int InvalidFrame(testLogger& log) {
    long mainTime, basicTime, rogueTime;
    Node n;
    CallStack stack(&n);
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
