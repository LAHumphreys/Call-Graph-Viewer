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

void CallgrindNative::AddFile(const int& id, const std::string& path ) {
    sources.emplace(id,SourceFile(path));
}

CallgrindNative::CallgrindNative(const std::string& fname) 
    : child(nullptr), current(&root), numCalls(0), currentFile(-1), childFile(-1), currentLine(0), currentActiveLine(0)
{

    // Default to not caring about this file...
    AddFile(-1,"/dev/null");

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
     *         fi=(199) [/var/tmp/portage/sys-libs/glibc-2.17/work/glibc-2.17/dlfcn/dlerror.c]
     *         fe=(199) [/var/tmp/portage/sys-libs/glibc-2.17/work/glibc-2.17/dlfcn/dlerror.c]
     *     File next child func is stored in:
     *         cfi=(92) [/var/tmp/portage/sys-libs/glibc-2.17/work/glibc-2.17/dlfcn/dlerror.c]
     *         cfl=(92) [/var/tmp/portage/sys-libs/glibc-2.17/work/glibc-2.17/dlfcn/dlerror.c]
     *
     *
     */

    string line;
    line.reserve(10240);
    while ( file.good() ) {
        char c = file.peek();
        if ( c == '*' || c == '+' || c== '-' || ( c >= '0' && c <= '9') ) {
            line.clear();
            std::getline(file,line);
            AddCost(line);
            // a cost line
        } else if ( c == 'f' || c == 'c' ) {
            // Could be a defn, a file or a func
            line.clear();
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
                } else if ( token == "cfl=" || token == "cfi=" ) {
                    SetChildFile(line);
                } else if ( token == "fl=(" || token == "fi=(" || token == "fe=(" ) {
                    ChangeFile(line);
                } else {
                    SLOG_FROM(LOG_VERY_VERBOSE, "CallgrindNative::CallgrindNative", "Skipped line (no token match) : " << line)
                }
            }
        } else {
            file.ignore(numeric_limits<streamsize>::max(),'\n');
            SLOG_FROM(LOG_VERY_VERBOSE, "CallgrindNative::CallgrindNative", "Skipped line (wrong c) : " << c)
        }

    }
}

//        01234
// Format fn=(<ID>) [fname'parent'...'root]
void CallgrindNative::SetCurrentFunction ( const std::string& line) {
    size_t id_end = line.find_first_of(')');
    int id = atoi(line.c_str()+4);
    auto it = idMap.find(id);
    Path path("");
    if ( it == idMap.end() ) {
        if ( line.find_first_of(' ') != string::npos ) {
            // Haven't seen this before, need the path...
              
            /*
             * We're going to work backwards from the end of the string,
             * this is equivalent of going down the call stack, starting at
             * the root node.
             *
             * We avoid constructing the std::string until the last
             * minute to avoid construction overhead if it is not necessary
             */
            size_t index = id_end;
            size_t last_index = line.length()-1;
            bool foundMain = false;
            for ( index = line.find_last_of('\'');
                  index != string::npos;
                  index = line.find_last_of('\'',index-1))
            {
                const char* token = line.c_str() + index +1;
                if (    !foundMain 
                     && (last_index - index) == 4 
                     && strncmp(token,"main",4) == 0 ) 
                {
                    foundMain = true;
                }
                if ( foundMain ) {
                    path.Extend(string(token,(last_index - index)));
                }

                last_index = index-1;
            }

            // Finally pick up the actual function name
            string name(line.substr(id_end + 2,(last_index+1)-(id_end +2)));
            if ( foundMain || name == "main" ) {
                // Place the node in the graph
                if ( name == "main" ) {
                    current = root.CreateNode(Path(""),name);
                } else {
                    current = root.CreateNode(path,name);
                }

                // Link the ID to the node
                idMap.emplace(id,current);

                // Link the node to the source file...
                current->SourceId() = currentFile;
                 
                SLOG_FROM(LOG_VERBOSE,"CallgrindNative::SetCurrentFunction", "Found new func: " << current->Name() << " ( " << id << " ) " << endl  << "from: " << line << endl << "File: " << currentFile << endl << "Parent: " << current->Parent()->Name())
            } else {
                SLOG_FROM(LOG_VERBOSE,"CallgrindNative::SetCurrentFunction", "Discarded: " << line << " because it is not under main" << "Name: " << name) current= &root;
            }
        }
    } else {
        // Alreay know about this function - nothing else to do...
        current = it->second;

        currentFile = current->SourceId();
        childFile = currentFile;

        SLOG_FROM(LOG_VERBOSE, "CallgrindNative::SetCurrentFunction", "Current is now : " << current->Name() << endl << "From: " << line )
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

            // We're not interested in stuff below main...
            if ( !(current == RootNode()) || name == "main" ) {
                // Create the child in the call graph
                child = current->MakeChild(name);

                // Link the function to the id
                idMap.emplace(id,child);

                // Link the function to a source file
                child->SourceId() = childFile;

                SLOG_FROM(LOG_VERBOSE,"CallgrindNative::CallChild", "Added new child func: " << child->Name() << " ( " << id <<" ) " << "Parent: " << child->Parent()->Name() << endl << "File: " << childFile << endl  << "from: " << line << endl )
            } else {
                SLOG_FROM(LOG_VERBOSE, "CallgrindNative::CallChild", " Not using child as its below main.." << name << endl << line)
            }
        } else {
            SLOG_FROM(LOG_VERBOSE, "CallgrindNative::CallChild", "Discarded: " << line << " because it has no name and we don't know the id")
        }

    } else {
        // Alreay know about this function - nothing else to do...
        child = it->second;

        SLOG_FROM(LOG_VERBOSE, "CallgrindNative::CallChild", "Child is now : " << child->Name() << endl << "From: " << line )
    }

    // Reset the file pointer...
    childFile = currentFile;

}

// format: <location> <cost unit1> <cost unit2>
// Location : 
//    * : Unchanged
//    -N: N lines before the last line no
//    +N: N lines after the last line no
//    N : At line N
void CallgrindNative::AddCost(const std::string& line) {

    // Extract the line No...
    auto nstart = line.find_first_of(' ');

    // Due to relative offsets, we always need to know the current line no...
    if ( line[0] == '*' ) {
        // currentLine is unchanged
    } else if ( line[0] == '+' ) {
        // Relative offset
        currentLine += atoi(line.c_str()+1);
    } else if ( line[0] == '-' ) {
        // Relative offset
        currentLine -= atoi(line.c_str()+1);
    } else {
        // absolute position
        currentLine = atoi(line.c_str());
    }

    SLOG_FROM(LOG_VERBOSE, "CallgrindNative::AddCall", "Current line is now " << currentLine << " after line " << line)

    long cost = atol(line.c_str() + nstart + 1);

    // But only calculate the cost if we need it
    if ( current->SourceId() == currentFile  || !child.IsNull() ) {

        // Annotate the node...
        if ( current->SourceId() == currentFile ) {
            current->Annotations().AddAnnotation(currentLine,cost);
            currentActiveLine = currentLine;

            if ( current->SourceStart() > currentLine ) {
                current->SourceStart() = currentLine;

                SLOG_FROM(LOG_VERBOSE, "CallgrindNative::AddCall", current->Name() << " start is now " << currentLine)
            }
            if ( current->SourceEnd() < currentLine ) {
                current->SourceEnd() = currentLine;

                SLOG_FROM(LOG_VERBOSE, "CallgrindNative::AddCall", current->Name() << " stop is now " << currentLine)
            }
        }

        // If we're making a call, we need to add the cost to 
        // the relevant node...
        if ( !child.IsNull() ) {
            child->AddCall(cost,numCalls);
            counter.AddCall(child->Name(),cost,numCalls);
            child = nullptr;
            numCalls = 0;
        }
    } else {
        if ( !current.IsNull() ) {
            current->Annotations().AddAnnotation(currentActiveLine,cost);
        }
        SLOG_FROM(LOG_VERBOSE, "CallgrindNative::AddCall", "Ignoring cost line!: " << line )
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

// fl=(id) [path]
// fe=(id) [path]
// fi=(id) [path]
void CallgrindNative::ChangeFile(const std::string& line) {
    // Extract the ID...
    size_t id_end = line.find_first_of(')');
    currentFile = atoi(line.substr(4,(id_end - 4)).c_str());

    // If we don't know about it, add it to the map...
    auto it = sources.find(currentFile);
    if ( it == sources.end() ) {
        AddFile(currentFile, line.substr(id_end+2));

        SLOG_FROM(LOG_VERBOSE, "CallgrindNative::ChangeFile", "Current file is now (new): " << currentFile << "(" << line.substr(id_end+2) << ")" << endl << "Changed from line: " << line )
    } else {
        SLOG_FROM(LOG_VERBOSE, "CallgrindNative::ChangeFile", "Current file is now: " << currentFile << "(" << it->second.Name() << ")" << endl << "Changed from line: " << line )
    }

}

// cfl=(id) [path]
// cfi=(id) [path]
void CallgrindNative::SetChildFile(const std::string& line) {
    size_t id_end = line.find_first_of(')');
    childFile = atoi(line.substr(5,(id_end - 5)).c_str());
    auto it = sources.find(childFile);

    if ( it == sources.end() ) {
        AddFile(childFile, line.substr(id_end+2));

        SLOG_FROM(LOG_VERBOSE, "CallgrindNative::SetChildFile", "Child file is now (new): " << childFile << "(" << line.substr(id_end+2) << ")" << endl << "Changed from line: " << line )
    } else {
        SLOG_FROM(LOG_VERBOSE, "CallgrindNative::SetChildFile", "Child file is now: " << childFile << "(" << it->second.Name() << ")" << endl << "Changed from line: " << line )
    }

}

SourceFile& CallgrindNative::GetFile(const int& id ) {
    auto it = sources.find(id);

    // Fall back to /dev/null
    if ( it == sources.end() ) {
        it = sources.find(-1);
    }
    return it->second;
}

string CallgrindNative::Annotate(NodePtr node) {
    SourceFile& f = GetFile(node->SourceId());
    int start = node->SourceStart();
    if ( start <= 3 ) {
        start = 1;
    } else {
        start -= 3;
    }

    int stop = node->SourceEnd() +7;

    return f.Annotate(node->Annotations(),start,stop,node->RunTime());
}
