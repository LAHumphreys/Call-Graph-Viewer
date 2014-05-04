#include "tester.h"
#include "buildCallGraph.h"

int Example(testLogger& log);
int Graph(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Analysing example data...",Example).RunTest();
    Test("Building example call graph...",Graph).RunTest();
    return 0;
}

int Example(testLogger& log) {
    CallProfile profile;

    profile.ProcessFile("data/exampleInput.csv");

    string actual = profile.PrintResults();

    string expected = 
        "------------------------------------------------------------------------\n"
        "|-               Most Time Spent in Function                          -|\n"
        "------------------------------------------------------------------------\n"
        "|    Function Name                   | Calls  | Time(us)   | us/call   |\n"
        "------------------------------------------------------------------------\n"
        "| Really Long Name that is far too...| 1      | 500        | 500       |\n"
        "| Func1                              | 2      | 202        | 101       |\n"
        "| Func2                              | 1      | 200        | 200       |\n"
        "------------------------------------------------------------------------\n"
        "\n"
        "------------------------------------------------------------------------\n"
        "|-               Most Expensive Function Calls                        -|\n"
        "------------------------------------------------------------------------\n"
        "|    Function Name                   | Calls  | Time(us)   | us/call   |\n"
        "------------------------------------------------------------------------\n"
        "| Really Long Name that is far too...| 1      | 500        | 500       |\n"
        "| Func2                              | 1      | 200        | 200       |\n"
        "| Func1                              | 2      | 202        | 101       |\n"
        "------------------------------------------------------------------------\n";

    if ( actual != expected ) {
        log << "Unexpected output!" << endl;
        log << "Expected" << endl << expected;
        log << "Actual" << endl << actual;
        return 1;
    }
    return 0;
}

int Graph(testLogger& log) {
    CallProfile profile;
    profile.ProcessFile("data/exampleGraph.csv");
    string actual = profile.PrintGraph();
    /*
     * ROOT
     * |
     * main (1x800)---F1 (2x200)---F2 (2x30)--NULL
     *              |            |
     *              |            --F3 (1x60)--NULL
     *              --F3 (2x100)
     */

    string expected = 
       "ROOT\n"
       "    Calls: 0, Time: 0, Av. Time: 0\n"
       "    main\n"
       "        Calls: 1, Time: 800, Av. Time: 800\n"
       "        f1\n"
       "            Calls: 2, Time: 400, Av. Time: 200\n"
       "            f3\n"
       "                Calls: 1, Time: 60, Av. Time: 60\n"
       "            f2\n"
       "                Calls: 2, Time: 60, Av. Time: 30\n"
       "        f3\n"
       "            Calls: 2, Time: 200, Av. Time: 100\n";
    if ( actual != expected ) {
        log << "Unexpected output!" << endl;
        log << "Expected" << endl << expected;
        log << "Actual" << endl << actual;
        return 1;
    }
    return 0;
}
