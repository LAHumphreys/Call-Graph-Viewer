#include "callCount.h"
#include "nodeConfig.h"
#include <algorithm>
#include <utility>
#include <iomanip>
using namespace std;

CallCount::Calls::Calls() 
    : calls(0), costs(NodeConfig::Instance().NewCost())
{
}

CallCount::Calls::Calls(const Calls& rhs) 
    : costs(NodeConfig::Instance().NewCost())
{
    (*this) = rhs;
}

CallCount::Calls::Calls(Calls&& moveFrom) 
    : calls(moveFrom.calls), costs(moveFrom.costs)
{
    moveFrom.costs = nullptr;
}

CallCount::Calls& CallCount::Calls::operator=(const Calls& rhs)
{
    if ( costs == nullptr ) {
        costs = NodeConfig::Instance().NewCost();
    }
    calls = rhs.calls;
    for( size_t i = 0; i< costs->Size(); ++i ) {
        (*costs)[i] = (*rhs.costs)[i];
    }
    return *this;
}

CallCount::Calls::~Calls() {
    delete costs;
}

void CallCount::AddCall(const string& name, 
                        const long& usecs,
                        int   callCount) 
{
    auto it = fcalls.find(name);
    if ( it != fcalls.end() ) {
        it->second.calls += callCount;
        it->second[0] += usecs;
    } else {
        Calls& count = fcalls[name];
        count.calls = callCount;
        count[0] = usecs;
    }
}

std::string CallCount::PrintResults(unsigned tableSize) const {
    stringstream result("");
    static const unsigned name_width = 70;
    const std::string& unitName = NodeConfig::Instance().CostFactory()
                                     .GetName(0);
    result << "-----------------------------------------------------------------------------------------------------------\n";
    result << "|-               Most Time Spent in Function                                                             -|\n";
    result << "-----------------------------------------------------------------------------------------------------------\n";
    result << "|    Function Name                                                      | Calls  | " 
           << left << setw(11) << unitName   << "| " << left << setw(9) << unitName + "/call" << " |\n";
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

        const long& cost = it.second[0];
        result << "| ";
        result << left << setw(name_width) << name;
        result << "| ";
        result << setw(7) << it.second.calls;
        result << "| ";
        result << setw(11) << cost;
        result << "| ";
        result << setw(10) << (it.second.calls == 0 ? 
                                  0 : 
                                  cost / it.second.calls);
        result << "|\n";
    }

    // And complete the table;
    result << "-----------------------------------------------------------------------------------------------------------\n";

    result << "\n";
    result << "-----------------------------------------------------------------------------------------------------------\n";
    result << "|-               Most Expensive Function Calls                                                           -|\n";
    result << "-----------------------------------------------------------------------------------------------------------\n";
    result << "|    Function Name                                                      | Calls  | " 
           << left << setw(11) << unitName   << "| " << left << setw(9) << unitName + "/call" << " |\n";
    result << "-----------------------------------------------------------------------------------------------------------\n";

    // Now print each one...
    for ( const call_pair& it: mostTimePerCall ) {
        // Make sure the name doesn't overflow
        string name = it.first;
        if ( name.length() > name_width ) {
            name = name.substr(0,name_width -3) + "...";
        }

        const long& cost = it.second[0];
        result << "| ";
        result << left << setw(name_width) << name;
        result << "| ";
        result << setw(7) << it.second.calls;
        result << "| ";
        result << setw(11) << cost;
        result << "| ";
        result << setw(10) << (it.second.calls == 0 ?  
                                  0 : 
                                  cost / it.second.calls);
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
                          return lhs.second[0] > rhs.second[0];
                      });

    // Select the tableSize most expensive function in terms time per call
    partial_sort_copy(fcalls.begin(),fcalls.end(),
                      mostTimePerCall.begin(),mostTimePerCall.end(),
                      [] (const call_pair& lhs, const call_pair& rhs) -> bool {
                          return ( lhs.second.calls == 0 ? 
                                    0 : 
                                    lhs.second[0] / lhs.second.calls ) >
                                    (rhs.second.calls == 0 ? 
                                     0 : 
                                     rhs.second[0] / rhs.second.calls);
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
                                  return lhs.second[0]> rhs.second[0];
                              } else {
                                  // LHS matches => it is bigger
                                  return 1;
                              }
                          } else if (patternRegex.Search(rhs.first) ) {
                              // RHS matchnes => it is bigger
                              return 0;
                          } else {
                              return lhs.second[0] > rhs.second[0];
                          }
                      });

    // Select the tableSize most expensive function in terms time per call
    auto cost_gt = [] (const call_pair& lhs, const call_pair& rhs) -> bool {
        return ( lhs.second.calls == 0 ? 
                 0 : 
                 lhs.second[0] / lhs.second.calls 
               ) >
               ( rhs.second.calls == 0 ? 
                 0 : 
                 rhs.second[0] /rhs.second.calls);
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
        const std::string& unitName = NodeConfig::Instance().CostFactory()
                                         .GetName(0);

        output << "                 Most Time Spent in Function\n";
        output << "               ===============================\n";
        output << "  Calls      " << left << setw(8) << unitName  << "      " 
               << left << setw(9) << unitName  + "/call" << "      Name\n";
        output << "---------  -----------   -------------  --------\n";
        //         1234567890123456789012345678901234567890123456789
        //         0000000001111111111222222222233333333334444444444
        // Now print each one...
        for ( const call_pair& it: mostTotalTime ) {
            if ( !patternRegex.Search(it.first) ) {
                break;
            }
            PrintWideRow(output,it.first,it.second.calls, 
                                         it.second[0]);
        }
        output << endl << endl;

        output << "                 Most Expensive Function Calls\n";
        output << "               =================================\n";
        output << "  Calls      " << left << setw(8) << unitName  << "      " 
               << left << setw(9) << unitName + "/call" << "      Name\n";
        output << "---------  -----------   -------------  --------\n";

        for ( const call_pair& it: mostTimePerCall ) {
            if ( !patternRegex.Search(it.first) ) {
                break;
            }
            PrintWideRow(output,it.first,it.second.calls, 
                                         it.second[0]);
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
    const std::string& unitName = NodeConfig::Instance().CostFactory()
                                     .GetName(0);

    stringstream output;
    output << "                 Most Time Spent in Function\n";
    output << "               ===============================\n";
    output << "  Calls      " << left << setw(8) << unitName  << "      " 
           << left << setw(9) << unitName + "/call" << "      Name\n";
    output << "---------  -----------   -------------  --------\n";
    //         1234567890123456789012345678901234567890123456789
    //         0000000001111111111222222222233333333334444444444
    // Now print each one...
    for ( const call_pair& it: mostTotalTime ) {
        PrintWideRow(output,it.first,it.second.calls, 
                                     it.second[0]);
    }
    output << endl << endl;

    output << "                 Most Expensive Function Calls\n";
    output << "               =================================\n";
    output << "  Calls      " << left << setw(8) << unitName  << "      " 
           << left << setw(9) << unitName +"/call" << "      Name\n";
    output << "---------  -----------   -------------  --------\n";

    for ( const call_pair& it: mostTimePerCall ) {
        PrintWideRow(output,it.first,it.second.calls, 
                                     it.second[0]);
    }
    return output.str();
}
