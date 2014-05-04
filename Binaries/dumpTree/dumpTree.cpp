#include "logger.h"
#include "buildCallGraph.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;


int main(int argc, const char *argv[])
{
    if ( argc != 2 ) {
        cout << "Usage: profile <data file>" << endl;
        cout << "Usage: profile <data file> count" << endl;
        return 1;
    }

    LOG_FROM(LOG_OVERVIEW, "main", "Building Profile...")
    CallProfile profile;
    profile.ProcessFile(argv[1]);
    LOG_FROM(LOG_OVERVIEW, "main", "Profile built - program ready!")

    NodePtr activeNode = profile.RootNode();
    cout << activeNode->PrintResults(2,10000) << endl;
    cout << profile.PrintResults() << endl;
    return 0;
}
