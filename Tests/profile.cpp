#include "tester.h"
#include "buildCallGraph.h"

int Example(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Analysing example data...",Example).RunTest();
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
