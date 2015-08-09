#ifndef __PROFILER_LIBPROFDATA_STACK_H___
#define __PROFILER_LIBPROFDATA_STACK_H___

#include "util_time.h"
#include <forward_list>
#include "path.h"
#include "node.h"

class CallStack {
public:
    struct Frame {
        Time startTime;
        std::string name;
    };

    CallStack(NodePtr rootNode);

    /*
     * Add a frame to the stack, noting the start time so 
     * we can calculate the run time later
     */
    void AddFrame(const std::string& name, const Time& start);

    /*
     * Leave the current frame, if name is the name of the curent frame.
     *
     * Assuming the name was correct, usecs is populated with the execution
     * time of this stack frame. 
     *
     * Return a boolean indicating success
     */
    bool LeaveFrame(const std::string& name, 
                    const Time& leaveTime,
                    long& usecs,
                    bool longUnwind = false);

private:
    NodePtr            activeNode;
    std::vector<Frame> stack;
};
#endif
