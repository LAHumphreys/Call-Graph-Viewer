#include "tester.h"
#include "callgrindTree.h"
#include "nodeApp.h"

CallgrindNative native("../data/native/flist_costs.callgrind");

/* Structure:
        "ROOT (ROOT)\n" 
        "    Calls: 0, Time: 0, Av. Time: 0\n" 
        "    main (ROOT/main)\n" 
        "        Calls: 1, Time: 54, Av. Time: 54\n" 
        "        odds (ROOT/main/odds)\n" 
        "            Calls: 1, Time: 75, Av. Time: 75\n" 
        "            div (ROOT/main/odds/div)\n" 
        "                Calls: 3, Time: 45, Av. Time: 15\n" 
        "        evens (ROOT/main/evens)\n" 
        "            Calls: 1, Time: 65, Av. Time: 65\n" 
        "            div (ROOT/main/evens/div)\n" 
        "                Calls: 2, Time: 42, Av. Time: 21\n" 
        "                pos_div2 (ROOT/main/evens/div/pos_div2)\n" 
        "                    Calls: 1, Time: 7, Av. Time: 7\n" 
        "                pos_div4 (ROOT/main/evens/div/pos_div4)\n" 
        "                    Calls: 1, Time: 7, Av. Time: 7\n";
*/

int Initialisation(testLogger& log);
int CD(testLogger& log);
int PUSHD(testLogger& log);
int main(int argc, const char *argv[])
{
    Test("Initialising an app...",Initialisation).RunTest();
    Test("Testing pwd changing...",CD).RunTest();
    Test("Testing pushd / popd...",PUSHD).RunTest();
    return 0;
}

int Initialisation(testLogger& log) {
    TestTerminal tester; 
    NodeApp app(native.RootNode(),tester);
    app.PWD();
    if ( tester.OutputAsString() != "ROOT" ) {
        log << "Failed to initialised in the root node: " << tester.OutputAsString();
        return 1;
    }
    return 0;
}

#define SCRIPT(descr,output,code) \
    tester.Reset(); \
    code \
    if ( tester.OutputAsString() != output ) { \
        log << descr << endl; \
        log << "Exected: >" output << "<" << endl; \
        log << "Actual: >" << tester.OutputAsString() <<"<"; \
        return 1; \
    }

int CD(testLogger& log) {
    TestTerminal tester; 
    NodeApp app(native.RootNode(),tester);
    SCRIPT ("Change into main", "ROOT/\n  main", 
        app.CD("main");
        app.PWD();
    )
    SCRIPT ("change down two levels","ROOT/\n  main/\n  odds/\n  div",
        app.CD("odds/div");
        app.PWD();
    )
    SCRIPT ("Handle an empty path","error, no such node:",
        app.CD("");
    )
    SCRIPT ("Handle a space path","error, no such node:" ,
        app.CD(" ");
    )
    SCRIPT ("No path change after invalid paths", "ROOT/\n  main/\n  odds/\n  div",
        app.PWD();
    )
    SCRIPT ("Changing via the root node", "ROOT/\n  main/\n  evens/\n  div/\n  pos_div2", 
        app.CD("ROOT/main/evens/div/pos_div2");
        app.PWD();
    )
    SCRIPT ("Change up two levels", "ROOT/\n  main/\n  evens",
        app.CD("../..");
        app.PWD();
    )
    SCRIPT("Changing up to many levels","error, no such node: ../../../../..",
        app.CD("../../../../..");
    )
    SCRIPT("pwd after invalid ..s","ROOT/\n  main/\n  evens",
       app.PWD();
    )
    SCRIPT("CD via parent's parent","ROOT/\n  main/\n  odds/\n  div",
       app.CD("../../main/odds/div");
       app.PWD();
    )
    return 0;
}

int PUSHD(testLogger& log) {
    TestTerminal tester; 
    NodeApp app(native.RootNode(),tester);
    
    SCRIPT ("Push root on to the stack", "ROOT/main\nROOT\nROOT/\n  main", 
        app.PUSHD("main");
        app.PWD();
    )
    SCRIPT ("Push odds onto the statck","main/odds\nROOT/main\nROOT",
        app.PUSHD("odds");
        app.CD("ROOT/main/evens");
    )
    SCRIPT ("pop the stack","ROOT/main\nROOT\nROOT/\n  main",
        app.POPD();
        app.PWD();
    )
    SCRIPT ("pop the stack again","ROOT\nROOT",
        app.POPD();
        app.PWD();
    )
    SCRIPT ("pop an empty stack","error: Node stack is empty!",
        app.CD("main");
        app.POPD();
    )
    return 0;
}
