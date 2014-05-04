#include "buildCallGraph.h"
#include <iostream>

using namespace std;

int main(int argc, const char *argv[])
{
    int tableSize = 0;
    if ( argc < 2 || argc > 3 ) {
        cout << "Usage: profile <data file>" << endl;
        cout << "Usage: profile <data file> count" << endl;
        return 1;
    } else if ( argc ==3 ) {
        tableSize = atoi(argv[2]);
    }

    CallProfile profile;

    profile.ProcessFile(argv[1]);

    cout << profile.PrintResults(tableSize) << endl;
    cout << profile.PrintGraph() << endl;
    return 0;
}
