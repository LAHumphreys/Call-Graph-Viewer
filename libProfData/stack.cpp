#include "stack.h"

using namespace std;

CallStack::CallStack() {
    stack.reserve(200);
}

bool CallStack::LeaveFrame(const std::string& name, 
                           const Time& leaveTime,
                           long& usecs) {
    bool success = false;

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
    }

    return success;
}
