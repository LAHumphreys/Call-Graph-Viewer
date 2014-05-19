#include "callCount.h"
#include <algorithm>
#include <utility>
#include <iomanip>
using namespace std;

void CallCount::AddCall(const string& name, 
                        const long& usecs,
                        int   callCount) 
{
    auto it = fcalls.find(name);
    if ( it != fcalls.end() ) {
        it->second.calls += callCount;
        it->second.usecs += usecs;
    } else {
        Calls& count = fcalls[name];
        count.calls = callCount;
        count.usecs = usecs;
    }
}

std::string CallCount::PrintResults(unsigned tableSize) const {
    stringstream result("");
    static const unsigned name_width = 70;
    result << "-----------------------------------------------------------------------------------------------------------\n";
    result << "|-               Most Time Spent in Function                                                             -|\n";
    result << "-----------------------------------------------------------------------------------------------------------\n";
    result << "|    Function Name                                                      | Calls  | Time(us)   | us/call   |\n";
    result << "-----------------------------------------------------------------------------------------------------------\n";
    //           <-          35                   ->  <- 7 ->  <- 11    ->  <- 10   ->
    // Sort into order of most expensive...
    vector<call_pair> mostTotalTime;
    vector<call_pair> mostTimePerCall;

    if ( tableSize == 0 ) {
        tableSize = fcalls.size();
    }

    PopulateTables(tableSize, mostTotalTime,mostTimePerCall);


    // Now print each one...
    for ( const call_pair& it: mostTotalTime ) {
        // Make sure the name doesn't overflow
        string name = it.first;
        if ( name.length() > name_width ) {
            name = name.substr(0,name_width -3) + "...";
        }

        result << "| ";
        result << left << setw(name_width) << name;
        result << "| ";
        result << setw(7) << it.second.calls;
        result << "| ";
        result << setw(11) << it.second.usecs;
        result << "| ";
        result << setw(10) << (it.second.calls == 0 ? 
                                  0 : 
                                  it.second.usecs / it.second.calls);
        result << "|\n";
    }

    // And complete the table;
    result << "-----------------------------------------------------------------------------------------------------------\n";

    result << "\n";
    result << "-----------------------------------------------------------------------------------------------------------\n";
    result << "|-               Most Expensive Function Calls                                                           -|\n";
    result << "-----------------------------------------------------------------------------------------------------------\n";
    result << "|    Function Name                                                      | Calls  | Time(us)   | us/call   |\n";
    result << "-----------------------------------------------------------------------------------------------------------\n";

    // Now print each one...
    for ( const call_pair& it: mostTimePerCall ) {
        // Make sure the name doesn't overflow
        string name = it.first;
        if ( name.length() > name_width ) {
            name = name.substr(0,name_width -3) + "...";
        }

        result << "| ";
        result << left << setw(name_width) << name;
        result << "| ";
        result << setw(7) << it.second.calls;
        result << "| ";
        result << setw(11) << it.second.usecs;
        result << "| ";
        result << setw(10) << (it.second.calls == 0 ?  
                                  0 : 
                                  it.second.usecs / it.second.calls);
        result << "|\n";
    }

    // And complete the table;
    result << "-----------------------------------------------------------------------------------------------------------\n";
    return result.str();
};

void CallCount::PopulateTables( unsigned tableSize, 
                                vector<call_pair>& mostTotalTime, 
                                vector<call_pair>& mostTimePerCall) const
{
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
                          return ( lhs.second.calls == 0 ? 0 : lhs.second.usecs/lhs.second.calls ) >
                                 ( rhs.second.calls == 0 ? 0 : rhs.second.usecs/rhs.second.calls);
                      });
}

void CallCount::PopulateTables( unsigned tableSize, 
                                vector<call_pair>& mostTotalTime, 
                                vector<call_pair>& mostTimePerCall,
                                const RegPattern& patternRegex) const
{

    mostTotalTime.resize(tableSize);
    mostTimePerCall.resize(tableSize);

    // Select the tableSize most expensive function in terms of total time
    partial_sort_copy(fcalls.begin(),fcalls.end(),
                      mostTotalTime.begin(),mostTotalTime.end(),
                      [=, &patternRegex] (const call_pair& lhs, const call_pair& rhs) -> bool {
                          if ( patternRegex.Search(lhs.first) ) 
                          {
                              if (patternRegex.Search(rhs.first) ) {
                                  // Both match, check them
                                  return lhs.second.usecs>rhs.second.usecs;
                              } else {
                                  // LHS matches => it is bigger
                                  return 1;
                              }
                          } else if (patternRegex.Search(rhs.first) ) {
                              // RHS matchnes => it is bigger
                              return 0;
                          } else {
                              return lhs.second.usecs>rhs.second.usecs;
                          }
                      });

    // Select the tableSize most expensive function in terms time per call
    auto cost_gt = [] (const call_pair& lhs, const call_pair& rhs) -> bool {
        return ( lhs.second.calls == 0 ? 0 : lhs.second.usecs/lhs.second.calls ) >
               ( rhs.second.calls == 0 ? 0 : rhs.second.usecs/rhs.second.calls);
    };

    partial_sort_copy(fcalls.begin(),fcalls.end(),
                      mostTimePerCall.begin(),mostTimePerCall.end(),
                      [=, &patternRegex] (const call_pair& lhs, const call_pair& rhs) -> bool {
                          if ( patternRegex.Search(lhs.first)) 
                          {
                              if (patternRegex.Search(rhs.first)) {
                                  // Both match, check them
                                  return cost_gt(lhs,rhs);
                              } else {
                                  // LHS matches => it is bigger
                                  return 1;
                              }
                          } else if (patternRegex.Search(rhs.first)) {
                              // RHS matchnes => it is bigger
                              return 0;
                          } else {
                              return cost_gt(lhs,rhs);
                          }
                      });
}

void CallCount::PrintWideRow(stringstream& output,
                             const std::string& name, 
                             const int& calls, 
                             const long& usecs) const
{
        output << " " << left << setw(10) << calls;
        output << " " << setw(11) << usecs;
        output << "   " << setw(13) << (calls == 0 ?  
                                          0 : 
                                          usecs / calls);
        output << "  " << name << endl;
}

string CallCount::FilteredPrint(const string& pattern, unsigned tableSize) const {

    stringstream output;
    vector<call_pair> mostTotalTime;
    vector<call_pair> mostTimePerCall;

    if ( tableSize == 0 ) {
        tableSize = fcalls.size();
    }

    try {
        RegPattern patternRegex(pattern);


        PopulateTables(tableSize, mostTotalTime,mostTimePerCall, patternRegex);

        output << "                 Most Time Spent in Function\n";
        output << "               ===============================\n";
        output << "  Calls      Time(us)      us/call        Name\n";
        output << "---------  -----------   -------------  --------\n";
        //         1234567890123456789012345678901234567890123456789
        //         0000000001111111111222222222233333333334444444444
        // Now print each one...
        for ( const call_pair& it: mostTotalTime ) {
            if ( !patternRegex.Search(it.first) ) {
                break;
            }
            PrintWideRow(output,it.first,it.second.calls, it.second.usecs);
        }
        output << endl << endl;

        output << "                 Most Expensive Function Calls\n";
        output << "               =================================\n";
        output << "  Calls      Time(us)      us/call        Name\n";
        output << "---------  -----------   -------------  --------\n";

        for ( const call_pair& it: mostTimePerCall ) {
            if ( !patternRegex.Search(it.first) ) {
                break;
            }
            PrintWideRow(output,it.first,it.second.calls, it.second.usecs);
        }
    } catch ( RegError& e ) {
        output << "Invalid regular expression: \n";
        output << e.what();
    }


    return output.str();
}

std::string CallCount::WidePrint(unsigned tableSize) const {
    vector<call_pair> mostTotalTime;
    vector<call_pair> mostTimePerCall;

    if ( tableSize == 0 ) {
        tableSize = fcalls.size();
    }

    PopulateTables(tableSize, mostTotalTime,mostTimePerCall);

    stringstream output;
    output << "                 Most Time Spent in Function\n";
    output << "               ===============================\n";
    output << "  Calls      Time(us)      us/call        Name\n";
    output << "---------  -----------   -------------  --------\n";
    //         1234567890123456789012345678901234567890123456789
    //         0000000001111111111222222222233333333334444444444
    // Now print each one...
    for ( const call_pair& it: mostTotalTime ) {
        PrintWideRow(output,it.first,it.second.calls, it.second.usecs);
    }
    output << endl << endl;

    output << "                 Most Expensive Function Calls\n";
    output << "               =================================\n";
    output << "  Calls      Time(us)      us/call        Name\n";
    output << "---------  -----------   -------------  --------\n";

    for ( const call_pair& it: mostTimePerCall ) {
        PrintWideRow(output,it.first,it.second.calls, it.second.usecs);
    }
    return output.str();
}
