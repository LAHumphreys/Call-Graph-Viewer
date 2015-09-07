#include "stack.h"
#include "logger.h"

using namespace std;

CallStack::CallStack(NodePtr _rootNode) 
    : activeNode(_rootNode)
{
    stack.reserve(200);
}

void CallStack::AddFrame(const std::string& name, const Time& start) 
{
    activeNode = activeNode->MakeChild(name);

    stack.emplace_back(Frame{start, name});
}

bool CallStack::LeaveFrame(const std::string& name, 
                           const Time& leaveTime,
                           long& usecs,
                           bool longUnwind)
{
    bool success = false;

            SLOG_FROM(LOG_DEFAULT, "CallStack::LeaveFrame",
               "Leaving: " << name)

    if ( stack.size() > 0 ) {
        Frame& frame = stack.back();
        // If its the correct frame pop it...
        if ( frame.name == name ) {
            // Update the node in the call graph
            usecs = leaveTime.DiffUSecs(frame.startTime);
            activeNode->AddCall(usecs);

            // Remove the frame from the stack
            stack.pop_back();
            if ( !activeNode->IsRoot() ) {
                activeNode = activeNode->Parent();
            }

            success = true;
        } else {
            if (longUnwind) {
                success = true;
                Frame* pframe = &frame;
                bool found = false;
                do
                {
                    pframe = &stack.back();
                    found = (pframe->name == name);
                    success &= LeaveFrame(pframe->name,leaveTime,usecs,false);
                } while (stack.size() > 0 && !found);

                if (!found) {
                    success = false;
                }

            } else {
                usecs = -1;
                SLOG_FROM(LOG_ERROR, "CallStack::LeaveFrame",
                "Failed to leave frame " << name << " does not match statck " << stack.back().name;)
            }
        }
    } else {
            SLOG_FROM(LOG_ERROR, "CallStack::LeaveFrame",
               "Failed to leave frame " << name << " because the stack is empty!")
    }


    return success;
}
