#include "stack.h"
#include "logger.h"

using namespace std;

CallStack::CallStack() {
    stack.reserve(200);
}

bool CallStack::LeaveFrame(const std::string& name, 
                           const Time& leaveTime,
                           long& usecs) {
    bool success = false;

    if ( stack.size() > 0 ) {
        // Get the current frame
        Frame frame(stack.back());

        // If its the correct frame pop it...
        if ( frame.name == name ) {
            usecs = leaveTime.DiffUSecs(frame.startTime);
            stack.pop_back();
            success = true;
        } else {
        // Otherwise there's isn't anything we can do
            usecs = -1;
            SLOG_FROM(LOG_ERROR, "CallStack::LeaveFrame",
               "Failed to leave frame " << name << " does not match statck " << frame.name;)
        }
    } else {
            SLOG_FROM(LOG_ERROR, "CallStack::LeaveFrame",
               "Failed to leave frame " << name << " because the stack is empty!")
    }


    return success;
}
