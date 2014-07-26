#include "tester.h"
#include "node.h"
#include "nodeSearchCache.h"
#include "callgrindTree.h"

int Table(testLogger& log);
int Tree(testLogger& log);
int Annotate(testLogger& log);
int CheckUnits(testLogger& log);

int main(int argc, const char *argv[])
{

    Test("Checking units loaded correctly...",CheckUnits).RunTest();
    return 0;
}

int CheckUnits(testLogger& log) {
    CallgrindNative native("data/toy_cache.data");
    vector<string> events = { "Ir", "Dr", "Dw", "I1mr", "D1mr", "D1mw", "ILmr", "DLmr", "DLmw" };
    for ( size_t i = 0; i < events.size(); ++i ) {
        if ( native.GetCostIdx(events[i]) != i ) {
            log << events[i] + " should be at index " << i << "!" << endl;
            return 1;
        }
    }
    StringStruct costs = native.MakeCosts("1 0 1 1 0 0 1");
    return 0;
}

int Table ( testLogger& log ) {
    CallgrindNative native("toy.data");
    string expected = 
"                 Most Time Spent in Function\n"
"               ===============================\n"
"  Calls      Time(us)      us/call        Name\n"
"---------  -----------   -------------  --------\n"
" 1          4417          4417           main\n"
" 100        2500          25             quad_it\n"
" 300        2100          7              double_it\n"
"\n"
"\n"
"                 Most Expensive Function Calls\n"
"               =================================\n"
"  Calls      Time(us)      us/call        Name\n"
"---------  -----------   -------------  --------\n"
" 1          4417          4417           main\n"
" 100        2500          25             quad_it\n"
" 300        2100          7              double_it\n";

    string actual = native.Counter().WidePrint();

    if ( expected != actual ) {
        log << " Failed to read callgrind table!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int Tree ( testLogger& log ) {
    CallgrindNative native("toy.data");
    string actual = native.RootNode()->PrintResults(2,100,false);
    string expected = 
"  ROOT\n"
"      Calls: 0, Time: 0, Av. Time: 0\n"
"      main\n"
"          Calls: 1, Time: 4417, Av. Time: 4417\n"
"          quad_it\n"
"              Calls: 100, Time: 2500, Av. Time: 25\n"
"              double_it\n"
"                  Calls: 200, Time: 1400, Av. Time: 7\n"
"          double_it\n"
"              Calls: 100, Time: 700, Av. Time: 7\n";
    if ( expected != actual ) {
        log << " Failed to read callgrind tree!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}

int Annotate ( testLogger& log ) {
    CallgrindNative native("toy.data");
    NodePtr main  = native.RootNode()->GetNode(Path("main"));
    string actual = native.Annotate(main);
    string expected = 
"    8:            ( 0%): }\n"
"    9:            ( 0%): \n"
"   10:            ( 0%): int main(int argc, const char *argv[])\n"
"   11:          5 ( 0%): {\n"
"   12:          1 ( 0%):     int i =0;\n"
"   13:        304 ( 6%):     for ( i = 0; i <100; i ++ ) {\n"
"   14:       1000 (22%):         double_it(i);\n"
"   15:            ( 0%):     }\n"
"   16:        304 ( 6%):     for ( i = 0; i <100; i ++ ) {\n"
"   17:       2800 (63%):         quad_it(i);\n"
"   18:            ( 0%):     }\n"
"   19:          1 ( 0%):     return 0;\n"
"   20:          2 ( 0%): }\n";
    if ( expected != actual ) {
        log << " Failed to read source!";
        log << " Expected: " << endl;
        log << ">" << expected << "<" << endl;
        log << " Actual: " << endl;
        log << ">" << actual << "<" ;
        return 1;
    }
    return 0;
}
