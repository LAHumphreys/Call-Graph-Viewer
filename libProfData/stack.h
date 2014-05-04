#ifndef __PROFILER_LIBPROFDATA_STACK_H___
#define __PROFILER_LIBPROFDATA_STACK_H___

#include "util_time.h"
#include <vector>
#include "path.h"

class CallStack {
public:
    struct Frame {
        Time startTime;
        std::string name;
    };

    CallStack();

    /*
     * Add a frame to the stack, noting the start time so 
     * we can calculate the run time later
     */
    void AddFrame(const std::string& name, const Time& start) {
        stack.push_back({start,name});
        path.push_back(name);
    }

    /*
     * Leave the current frame, if name is the name of the curent frame.
     *
     * Assuming the name was correct, usecs is populated with the execution
     * time of this stack frame. 
     *
     * path is populated with the path to the parent
     *
     * Return a boolean indicating success
     */
    bool LeaveFrame(const std::string& name, 
                    const Time& leaveTime,
                    long& usecs,
                    Path& pathToParent);

private:
    std::vector<Frame> stack;
    std::vector<std::string> path;
};
#endif
