#include "tester.h"
#include "callgrindTree.h"
#include "nodeApp.h"
#include "nodeUserApp.h"

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
int Scripting(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Initialising an app...",Initialisation).RunTest();
    Test("Testing pwd changing...",CD).RunTest();
    Test("Testing pushd / popd...",PUSHD).RunTest();
    Test("Doing some basic scripting...",Scripting).RunTest();
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

#define DIRECT_CALL(descr,output,code) \
    tester.Reset(); \
    code \
    if ( tester.OutputAsString() != output ) { \
        log << descr << endl; \
        log << "exected: >" output << "<" << endl; \
        log << "actual: >" << tester.OutputAsString() <<"<"; \
        return 1; \
    }

#define SCRIPT(descr,output,code) \
    tester.Reset(); \
    app.Execute(CommandScript(code)); \
    if ( tester.OutputAsString() != output ) { \
        log << descr << endl; \
        log << "exected: >" << output << "<" << endl; \
        log << "actual: >" << tester.OutputAsString() <<"<"; \
        return 1; \
    }

int CD(testLogger& log) {
    TestTerminal tester; 
    NodeApp app(native.RootNode(),tester);
    DIRECT_CALL ("Change into main", "ROOT/\n  main", 
        app.CD("main");
        app.PWD();
    )
    DIRECT_CALL ("change down two levels","ROOT/\n  main/\n  odds/\n  div",
        app.CD("odds/div");
        app.PWD();
    )
    DIRECT_CALL ("Handle an empty path","error, no such node: ^",
        app.CD("");
    )
    DIRECT_CALL ("Handle a space path","error, no such node: ^" ,
        app.CD(" ");
    )
    DIRECT_CALL ("No path change after invalid paths", "ROOT/\n  main/\n  odds/\n  div",
        app.PWD();
    )
    DIRECT_CALL ("Changing via the root node", "ROOT/\n  main/\n  evens/\n  div/\n  pos_div2", 
        app.CD("ROOT/main/evens/div/pos_div2");
        app.PWD();
    )
    DIRECT_CALL ("Change up two levels", "ROOT/\n  main/\n  evens",
        app.CD("../..");
        app.PWD();
    )
    DIRECT_CALL("Changing up to many levels","error, no such node: ^../../../../..",
        app.CD("../../../../..");
    )
    DIRECT_CALL("pwd after invalid ..s","ROOT/\n  main/\n  evens",
       app.PWD();
    )
    DIRECT_CALL("CD via parent's parent","ROOT/\n  main/\n  odds/\n  div",
       app.CD("../../main/odds/div");
       app.PWD();
    )
    DIRECT_CALL ("Change into ROOT", "ROOT", 
        app.CD("ROOT");
        app.PWD();
    )
    return 0;
}

int PUSHD(testLogger& log) {
    TestTerminal tester; 
    NodeApp app(native.RootNode(),tester);
    
    DIRECT_CALL ("Push root on to the stack", "ROOT/main\nROOT\nROOT/\n  main", 
        app.PUSHD("main");
        app.PWD();
    )
    DIRECT_CALL ("Push odds onto the statck","main/odds\nROOT/main\nROOT",
        app.PUSHD("odds");
        app.CD("ROOT/main/evens");
    )
    DIRECT_CALL ("pop the stack","ROOT/main\nROOT\nROOT/\n  main",
        app.POPD();
        app.PWD();
    )
    DIRECT_CALL ("pop the stack again","ROOT\nROOT",
        app.POPD();
        app.PWD();
    )
    DIRECT_CALL ("pop an empty stack","error: Node stack is empty!",
        app.CD("main");
        app.POPD();
    )
    return 0;
}

int Scripting(testLogger& log) {
    TestTerminal tester;
    NodeUserApp app(native.RootNode(),tester);;

    string code = R"CODE(
        pushd main
        pwd
    )CODE";
    SCRIPT("Check starting node", "ROOT/main\nROOT\nROOT/\n  main",code)

    code = R"CODE(
        // Push odds onto the stack for safe keeping
        pushd odds

        // Change into evens, see if the stack is preserved...
        cd ROOT/main/evens
    )CODE";
    SCRIPT ("Push odds onto the statck","main/odds\nROOT/main\nROOT",code)

    code = R"CODE(
        alias with "pushd ${1}; ${2@}; popd"
        with ROOT pwd
        pwd
    )CODE";
string expected= R"(ROOT
main/evens
ROOT/main
ROOT
ROOT
main/evens
ROOT/main
ROOT
ROOT/
  main/
  evens)";
    SCRIPT ("Use with",expected,code)

    return 0;
}
