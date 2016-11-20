#ifndef __PROFILER_LIBPROFDATA_CALL_COUNT_H__
#define __PROFILER_LIBPROFDATA_CALL_COUNT_H__

#include <sstream>
#include <unordered_map>
#include <string>
#include <vector>
#include "regex_utils.h"
#include "stringStruct.h"

class CallCount {
public:
    // Initialise with no existing function calls mapped
    CallCount() {};

    struct Calls
    {
        Calls();
        Calls(const Calls& rhs);
        Calls(Calls&& moveFrom);
        Calls& operator=(const Calls& rhs);
        ~Calls();
        long& operator[](const size_t& idx) {
            return (*costs)[idx];
        }
        const long& operator[](const size_t& idx) const {
            return (*costs)[idx];
        }
        Calls& operator+=(const StringStruct& rhs) {
            *costs += rhs;
            return *this;
        }

        int calls;
        StringStruct* costs;
    };

    using call_pair = std::pair<std::string,Calls>;
    using SortedCallList = std::vector<call_pair>;

    /*
     * Register a new call with the class
     */
    void AddCall( const std::string& name, 
                  const StringStruct& costs,
                  int   count = 1);

    const Calls& GetCount(std::string name) {
        // This will zero-construct if the f doesn't exist...
        return fcalls[name];
    }

    std::string PrintResults(unsigned tableSize=0) const;

    std::string WidePrint(unsigned tableSize=0) const;

    std::string FilteredPrint(const std::string& pattern, unsigned tableSize = 0) const;

    void GetMostTotalTime(
        SortedCallList& sortedCalls,
        unsigned tableSize = 0) const;

    void GetMostTotalTime(
        SortedCallList& sortedCalls,
        const RegPattern& patternRegex,
        unsigned tableSize = 0) const;


private:
    /*
     * Helper function to draw a results table. If average is set to
     * true the costs will be averaged of the number of calls
     */
    void PrintTable(const std::string& name, 
                    const std::vector<call_pair>& rows,
                    bool  average,
                    std::stringstream& output) const;

    void PopulateTables(unsigned tableSize, 
                        std::vector<call_pair>& mostTotalTime, 
                        std::vector<call_pair>& mostTimePerCall) const;

    void PopulateTables(unsigned tableSize, 
                        std::vector<call_pair>& mostTotalTime, 
                        std::vector<call_pair>& mostTimePerCall,
                        const RegPattern& patternRegex) const;

    void PrintWideRow(std::stringstream& output,
                      const std::string& name, 
                      const int& calls, 
                      const long& usecs) const;

    void GetFilteredSelection(
        const RegPattern& patternRegex,
        std::vector<call_pair>& filteredCalls) const;

    void SortByMostTime(
        const std::vector<call_pair>& filteredCalls,
        std::vector<call_pair>& mostTotalTime,
        unsigned tableSize) const;

    std::unordered_map<std::string,Calls> fcalls;
  
};
#endif
