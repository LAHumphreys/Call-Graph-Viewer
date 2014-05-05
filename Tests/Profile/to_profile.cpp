#include "tester.h"
#include "buildCallGraph.h"

int Graph(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Analysing large data set...",Graph).RunTest();
    return 0;
}

int Graph(testLogger& log) {
    CallProfile profile;

    profile.ProcessFile("../data/largeData.csv");

    return 0;
}

