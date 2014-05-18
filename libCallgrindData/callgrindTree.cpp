#include "callgrindTree.h"
#include "stdReader.h"
#include "path.h"
#include "csv.h"
#include <fstream>
#include <string>
#include <limits>

using namespace std;

// id, name, path
using DataFile = CSV<int,string,Path>;
static const size_t ID = 0;
static const size_t FN_NAME = 1;
static const size_t PATH = 2;

// id, name, path
using CallsFile = CSV<int,int,int,long>;
static const size_t CALLER = 0;
static const size_t CALLED = 1;
static const size_t CALLS  = 2;
static const size_t COST   = 3;

// id, cost
using CostsFile = CSV<int,long>;
static const size_t TOTAL_COST = 1;

CallgrindCallTree::CallgrindCallTree (
    const string& fname)
       : root()
{
    IFStreamReader reader(fname.c_str());
    DataFile funcs(DataFile::LoadCSV(reader));

    for( int i =0; i< funcs.Rows(); ++i) {
        NodePtr node = root.CreateNode(
                               funcs.GetCell<PATH>(i),
                               funcs.GetCell<FN_NAME>(i));
        idMap.emplace(funcs.GetCell<ID>(i),node);
    }
}

void CallgrindCallTree::LoadCalls(const string& fname) {
    IFStreamReader reader(fname.c_str());
    CallsFile calls(CallsFile::LoadCSV(reader));

    /*
     * Loop over eached called function and add the count
     * / cost data.
     *
     * Since we have already graphed everything we can
     * actualyl discard the caller information (its
     * implicit)
     */
    for( int i =0; i< calls.Rows(); ++i) {
        auto it = idMap.find(calls.GetCell<CALLED>(i));
        if ( it != idMap.end() ) {
            NodePtr called = it->second;
            called->AddCall( calls.GetCell<COST>(i),
                             calls.GetCell<CALLS>(i));
        }
    }
}

void CallgrindCallTree::LoadCosts(const string& fname) {
    IFStreamReader reader(fname.c_str());
    CostsFile costs(CostsFile::LoadCSV(reader));

    /*
     * Loop over eached called function and add the count
     * / cost data.
     *
     * Since we have already graphed everything we can
     * actualyl discard the caller information (its
     * implicit)
     */
    for( int i =0; i< costs.Rows(); ++i) {
        auto it = idMap.find(costs.GetCell<ID>(i));
        if ( it != idMap.end() ) {
            NodePtr called = it->second;
            long cost = costs.GetCell<TOTAL_COST>(i);
            counter.AddCall( called->Name(), cost, 0);
        }
    }

    // Now we need to get the call count...
    root.ForEach([=] ( NodePtr&& node ) -> void {
        this->AddCalls(node);
    });
}

void CallgrindCallTree::AddCalls(NodePtr node) {
    counter.AddCall(node->Name(),
                    0,
                    node->Calls());
    node->ForEach([=] ( NodePtr&& node ) -> void {
        this->AddCalls(node);
    });
}

CallgrindNative::CallgrindNative(const std::string& fname) 
    : child(nullptr), current(&root), numCalls(0)
{
    DataFile funcs;
    CallsFile calls;
    CostsFile costs;
    ifstream file(fname);
    /* Please see the valgrind format definition:
     *    http://valgrind.org/docs/manual/cl-manual.html
     *
     * Types of line we have to handle:
     *
     * Cost lines: 
     *     <line No> <cost>
     *     where Line No may be a number, '*' to indicate as above, or
     *     '+N' to indicated a relavtive offset
     *
     *  Function Definitions: 
     *     Functions not seen before:
     *         fn=(<id>) <func name>'<parent>'....'<main>'....'<root>'
     *         cfn=(<id>) <func name>'<parent>'....'<main>'....'<root>'
     *     Functions seen before:
     *         fn=(<id>)   // Start of a new cost block
     *         cfn=(<id>)  // A call to a child function
     *  Calls (to the previous cfn line): 
     *         calls=<N> <offset>
     *
     * Types of line we are ignoring for the time being:
     *     Definitions:
     *        <property>: <value>
     *
     *     Object Files:
     *         ob=(6) /lib64/libdl-2.17.so
     *         cob=(3) /lib64/libc-2.17.so
     *
     *     Files:
     *         fl=(199) [/var/tmp/portage/sys-libs/glibc-2.17/work/glibc-2.17/dlfcn/dlerror.c]
     *         cfi=(92) [/var/tmp/portage/sys-libs/glibc-2.17/work/glibc-2.17/dlfcn/dlerror.c]
     *
     *
     */

    while ( file.good() ) {
        char c = file.peek();
        if ( c == '*' || c == '+' || ( c >= '0' && c <= '9') ) {
            string line;
            std::getline(file,line);
            AddCost(line);
            // a cost line
        } else if ( c == 'f' || c == 'c' ) {
            // Could be a defn, a file or a func
            string line;
            std::getline(file,line);
            if ( line.length() > 4 ) {
                string&& token = line.substr(0,4);
                if ( token == "fn=(" ) {
                    // Changes the current function
                    SetCurrentFunction(line);
                } else if ( token == "cfn=" ) {
                    // Call to a child function
                    CallChild(line);
                } else if ( token == "call" ) {
                    SetCalls(line);
                }
            }
        } else {
            file.ignore(numeric_limits<streamsize>::max(),'\n');
        }

    }
}

//        01234
// Format fn=(<ID>) [fname'parent'...'root]
void CallgrindNative::SetCurrentFunction ( const std::string& line) {
    size_t id_end = line.find_first_of(')');
    int id = atoi(line.substr(4,(id_end - 4)).c_str());
    auto it = idMap.find(id);
    Path path("");
    if ( it == idMap.end() ) {
        if ( line.find_first_of(' ') != string::npos ) {
            // Haven't seen this before, need the path...
            size_t index = id_end;
            size_t last_index = line.length()-1;
            bool foundMain = false;
            for ( index = line.find_last_of('\'');
                  index != string::npos;
                  index = line.find_last_of('\'',index-1))
            {
                string&& token = line.substr(index+1,last_index-1);
                last_index = index;
                if ( !foundMain && token == "main" ) {
                    foundMain = true;
                }
                path.Extend(token);
            }

            // Finally pick up the actual function name
            string name(line.substr(id_end + 2,(last_index+1)-(id_end +2)));

            if ( foundMain || name == "main" ) {
                current = root.CreateNode(path,name);
                idMap.emplace(id,current);
                 
                SLOG_FROM(LOG_VERY_VERBOSE,"CallgrindNative::SetCurrentFunction",
                           "Found new func: " << current->Name() << " ( " << id << " ) " 
                           << endl  << "from: " << line << endl
                           << "Parent: " << current->Parent()->Name())
            } else {
                SLOG_FROM(LOG_VERY_VERBOSE,"CallgrindNative::SetCurrentFunction",
                           "Discarded: " << line << " because there it is not under main")
            }
        }



    } else {
        // Alreay know about this function - nothing else to do...
        current = it->second;

        SLOG_FROM(LOG_VERY_VERBOSE, "CallgrindNative::SetCurrentFunction",
           "Current is now : " << current->Name() << endl
           << "From: " << line )
    }
}

//        01234
// Format cfn=(<ID>) [fname'parent'...'root]
void CallgrindNative::CallChild ( const std::string& line) {
    size_t id_end = line.find_first_of(')');
    int id = atoi(line.substr(5,(id_end - 5)).c_str());
    auto it = idMap.find(id);

    if ( it == idMap.end() ) {
        // We need the name, but not the path since we know that current must
        // be the parent...

        size_t name_end = line.find_first_of('\'');
        if ( name_end == string::npos ) {
            if ( line.find_first_of(' ') != string::npos ) {
                name_end = line.length();
            }
        }

        if ( name_end != string::npos ) {
            // Finally pick up the actual function name
            string name(line.substr(id_end +2,(name_end)-(id_end +2)));

            child = current->MakeChild(name);
            idMap.emplace(id,child);

            SLOG_FROM(LOG_VERY_VERBOSE,"CallgrindNative::CallChild",
                       "Added new child func: " << child->Name() << " ( " << id <<" ) " 
                       << "Parent: " << child->Parent()->Name()
                       << endl  << "from: " << line << endl )
        } else {
            SLOG_FROM(LOG_VERY_VERBOSE, "CallgrindNative::CallChild",
                       "Discarded: " << line << " because it has no name and we don't know the id")
        }

    } else {
        // Alreay know about this function - nothing else to do...
        child = it->second;

        SLOG_FROM(LOG_VERY_VERBOSE, "CallgrindNative::CallChild",
           "Child is now : " << child->Name() << endl
           << "From: " << line )
    }
}

// format: <location> <cost>
void CallgrindNative::AddCost(const std::string& line) {
    if ( !child.IsNull() ) {
        auto nstart = line.find_first_of(' ');
        auto nend = line.find_first_of(' ',nstart+1);
        if( nend == string::npos) {
            nend = line.length();
        }

        long cost = atol(line.substr(nstart+1,(nend-nstart-1)).c_str());

        child->AddCall(cost,numCalls);
        counter.AddCall(child->Name(),cost,numCalls);

        child = nullptr;
        numCalls = 0;
    }

}

// calls=<N> <offset>
void CallgrindNative::SetCalls(const string& line) {
    auto nstart = line.find_first_of('=');
    auto nend = line.find_first_of(' ');
    if ( nstart != string::npos && nend != string::npos ) {
        numCalls = atoi(line.substr(nstart+1,(nend - nstart-1)).c_str());
    }
}

