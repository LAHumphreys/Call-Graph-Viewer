#ifndef __PROFILER_LIBPROFDATA_CALL_COUNT_H__
#define __PROFILER_LIBPROFDATA_CALL_COUNT_H__

#include <unordered_map>
#include <string>


class CallCount {
public:
    // Initialise with no existing function calls mapped
    CallCount() {};

    struct Calls
    {
        int calls;
        long usecs;
    };

    /*
     * Register a new call with the class
     */
    void AddCall(const std::string& name, const long& usecs);

    const Calls& GetCount(std::string name) {
        // This will zero-construct if the f doesn't exist...
        return fcalls[name];
    }

    std::string PrintResults(unsigned tableSize=0) const;
private:
    std::unordered_map<std::string,Calls> fcalls;
  
};
#endif
