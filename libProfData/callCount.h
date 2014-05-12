#ifndef __PROFILER_LIBPROFDATA_CALL_COUNT_H__
#define __PROFILER_LIBPROFDATA_CALL_COUNT_H__

#include "callCount.h"
#include <unordered_map>
#include <string>
#include <vector>


class CallCount {
public:
    // Initialise with no existing function calls mapped
    CallCount() {};

    struct Calls
    {
        int calls;
        long usecs;
    };

    using call_pair = std::pair<std::string,Calls>;

    /*
     * Register a new call with the class
     */
    void AddCall( const std::string& name, 
                  const long& usecs,
                  int   count = 1);

    const Calls& GetCount(std::string name) {
        // This will zero-construct if the f doesn't exist...
        return fcalls[name];
    }

    std::string PrintResults(unsigned tableSize=0) const;

    std::string WidePrint(unsigned tableSize=0) const;
private:
    void PopulateTables(unsigned tableSize, 
                        std::vector<call_pair>& mostTotalTime, 
                        std::vector<call_pair>& mostTimePerCall) const;

    std::unordered_map<std::string,Calls> fcalls;
  
};
#endif
