#include "tester.h"
#include "callCount.h"

using namespace std;

int Manual(testLogger& log);
int Print(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Manuallying adding calls...",Manual).RunTest();
    Test("Printing Results...",Print).RunTest();
    return 0;
}

int Manual(testLogger& log) {
    CallCount counter;
    counter.AddCall("Func1",100);
    counter.AddCall("Func2",200);
    counter.AddCall("Func1",101);
    counter.AddCall("main",500);

    // Now validate the times...
    auto main = counter.GetCount("main");
    auto f1 = counter.GetCount("Func1");
    auto f2 = counter.GetCount("Func2");
    auto unknown = counter.GetCount("XXX");

    if ( main.calls != 1 ) {
        log << "Invalid call count for main: " << main.calls;
        return 1;
    }
    if ( main.usecs != 500 ) {
        log << "Invalid call time for main : " << main.usecs;
        return 1;
    }

    if ( f1.calls != 2 ) {
        log << "Invalid call count for f1: " << f1.calls;
        return 1;
    }
    if ( f1.usecs != 201 ) {
        log << "Invalid call time for f1 : " << f1.usecs;
        return 1;
    }

    if ( f2.calls != 1 ) {
        log << "Invalid call count for f2: " << f2.calls;
        return 1;
    }
    if ( f2.usecs != 200 ) {
        log << "Invalid call time for f2 : " << f2.usecs;
        return 1;
    }

    if ( unknown.calls != 0 ) {
        log << "Invalid call count for unknown: " << unknown.calls;
        return 1;
    }
    if ( unknown.usecs != 0 ) {
        log << "Invalid call time for unknown : " << unknown.usecs;
        return 1;
    }

    return 0;
}

int Print(testLogger& log) {
    CallCount counter;
    counter.AddCall("Func1",100);
    counter.AddCall("Func2",200);
    counter.AddCall("Func1",102);
    counter.AddCall("Really Long Name that is far too long to fit in the box, no really it is really long",500);

    string expected = 
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Time Spent in Function                                                             -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time(us)   | us/call   |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| Really Long Name that is far too long to fit in the box, no really ...| 1      | 500        | 500       |\n"
"| Func1                                                                 | 2      | 202        | 101       |\n"
"| Func2                                                                 | 1      | 200        | 200       |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Expensive Function Calls                                                           -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time(us)   | us/call   |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| Really Long Name that is far too long to fit in the box, no really ...| 1      | 500        | 500       |\n"
"| Func2                                                                 | 1      | 200        | 200       |\n"
"| Func1                                                                 | 2      | 202        | 101       |\n"
"-----------------------------------------------------------------------------------------------------------\n";
    string expected_shorter = 
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Time Spent in Function                                                             -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time(us)   | us/call   |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| Really Long Name that is far too long to fit in the box, no really ...| 1      | 500        | 500       |\n"
"| Func1                                                                 | 2      | 202        | 101       |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|-               Most Expensive Function Calls                                                           -|\n"
"-----------------------------------------------------------------------------------------------------------\n"
"|    Function Name                                                      | Calls  | Time(us)   | us/call   |\n"
"-----------------------------------------------------------------------------------------------------------\n"
"| Really Long Name that is far too long to fit in the box, no really ...| 1      | 500        | 500       |\n"
"| Func2                                                                 | 1      | 200        | 200       |\n"
"-----------------------------------------------------------------------------------------------------------\n";
    string actual = counter.PrintResults();
    string shorter = counter.PrintResults(2); 

    if ( expected != actual ) {
        log << "Expected: " << endl << ">" << expected << "<";
        log << "Got: " << endl <<  ">" << actual << "<";
        return 1;
    }

    if ( shorter != expected_shorter ) {
        log << "Expected: " << endl << ">" << expected_shorter << "<";
        log << "Got: " << endl <<  ">" << shorter << "<";
        return 1;
    }
   return 0;
}
