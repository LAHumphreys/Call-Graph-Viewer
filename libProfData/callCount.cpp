#include "callCount.h"
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <iomanip>
using namespace std;

using call_pair = pair<string,CallCount::Calls>;

void CallCount::AddCall(const string& name, const long& usecs) {
    auto it = fcalls.find(name);
    if ( it != fcalls.end() ) {
        ++(it->second.calls);
        it->second.usecs += usecs;
    } else {
        Calls& count = fcalls[name];
        count.calls = 1;
        count.usecs = usecs;
    }
}

std::string CallCount::PrintResults(unsigned tableSize) const {
    stringstream result("");
    result << "------------------------------------------------------------------------\n";
    result << "|-               Most Time Spent in Function                          -|\n";
    result << "------------------------------------------------------------------------\n";
    result << "|    Function Name                   | Calls  | Time(us)   | us/call   |\n";
    result << "------------------------------------------------------------------------\n";
    //           <-          35                   ->  <- 7 ->  <- 11    ->  <- 10   ->
    // Sort into order of most expensive...
    vector<call_pair> mostTotalTime;
    vector<call_pair> mostTimePerCall;

    if ( tableSize == 0 ) {
        tableSize = fcalls.size();
    }

    mostTotalTime.resize(tableSize);
    mostTimePerCall.resize(tableSize);

    // Select the tableSize most expensive function in terms of total time
    partial_sort_copy(fcalls.begin(),fcalls.end(),
                      mostTotalTime.begin(),mostTotalTime.end(),
                      [] (const call_pair& lhs, const call_pair& rhs) -> bool {
                          return lhs.second.usecs>rhs.second.usecs;
                      });

    // Select the tableSize most expensive function in terms time per call
    partial_sort_copy(fcalls.begin(),fcalls.end(),
                      mostTimePerCall.begin(),mostTimePerCall.end(),
                      [] (const call_pair& lhs, const call_pair& rhs) -> bool {
                          return lhs.second.usecs/lhs.second.calls >
                                 rhs.second.usecs/rhs.second.calls;
                      });

    // Now print each one...
    for ( const call_pair& it: mostTotalTime ) {
        // Make sure the name doesn't overflow
        string name = it.first;
        if ( name.length() > 35 ) {
            name = name.substr(0,32) + "...";
        }

        result << "| ";
        result << left << setw(35) << name;
        result << "| ";
        result << setw(7) << it.second.calls;
        result << "| ";
        result << setw(11) << it.second.usecs;
        result << "| ";
        result << setw(10) << it.second.usecs / it.second.calls;
        result << "|\n";
    }

    // And complete the table;
    result << "------------------------------------------------------------------------\n";

    result << "\n";
    result << "------------------------------------------------------------------------\n";
    result << "|-               Most Expensive Function Calls                        -|\n";
    result << "------------------------------------------------------------------------\n";
    result << "|    Function Name                   | Calls  | Time(us)   | us/call   |\n";
    result << "------------------------------------------------------------------------\n";

    // Now print each one...
    for ( const call_pair& it: mostTimePerCall ) {
        // Make sure the name doesn't overflow
        string name = it.first;
        if ( name.length() > 35 ) {
            name = name.substr(0,32) + "...";
        }

        result << "| ";
        result << left << setw(35) << name;
        result << "| ";
        result << setw(7) << it.second.calls;
        result << "| ";
        result << setw(11) << it.second.usecs;
        result << "| ";
        result << setw(10) << it.second.usecs / it.second.calls;
        result << "|\n";
    }

    // And complete the table;
    result << "------------------------------------------------------------------------\n";
    return result.str();
};
