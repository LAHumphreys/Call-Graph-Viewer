// callgraph headers
#include "callgrindTree.h"
#include "nodeUserApp.h"
// utility headers
#include "env.h"
#include "OSTools.h"
#include "screen.h"
#include "util_time.h"
#include <iostream>
#include <cstdlib>

#define COUT(line) \
    {   \
        stringstream __buf; \
        __buf << line; \
        term.PutString(__buf.str()); \
    }   \

CallgrindNative* data;
NodeUserApp*     application;
int PrintAnnotation(); 

using namespace std;
int main(int argc, const char *argv[])
{
    Terminal& term = Screen::Instance().MainTerminal();
    /*
     * Sanity check command line args
     */
    if ( argc != 2 ) {
        cout << "Usage: cgv <data file>" << endl;
        return 1;
    }

    /*
     * Build the call graph
     */
    term.PutString("Building call graph...");

    Time start;
    data = new CallgrindNative(argv[1]);
    Time end;

    COUT ( "done ("<< end.DiffUSecs(start)/1000 << "ms)" << endl)
    /*
     * Initialise the actual application and load user configuration
     */
    application = new NodeUserApp(data->RootNode());
    application->CommandDispatcher().AddCommand("annotate",&PrintAnnotation);

    if ( ENV::IsSet("CGV_CONFIG_FILE") ) { 
        string rcpath = ENV::GetEnvString("CGV_CONFIG_FILE");
        COUT ("Loading " + rcpath + "...");
        start.SetNow();
        CommandFile rcfile(rcpath);
        application->Execute(rcfile);
        end.SetNow();
        COUT ( "done (" << end.DiffUSecs(start)/1000 << "ms)" << endl);
    } else {
        // Check the unix home directory for a config file
        string homercpath = OS::Join(ENV::GetEnvString("HOME"),".cgvrc");
        if ( OS::Exists(homercpath) ) {
            COUT ("Loading " + homercpath + "...");
            start.SetNow();
            CommandFile homerc(homercpath);
            application->Execute(homerc);
            end.SetNow();
            COUT ( "done (" << end.DiffUSecs(start)/1000 << "ms)" << endl);
        }

        // Check the local directory for a config file
        if ( OS::Exists(".cgvrc") ) {
            COUT ("Loading ./.cgvrc ...");
            start.SetNow();
            CommandFile localrc(".cgvrc");
            application->Execute(localrc);
            end.SetNow();
            COUT ( "done (" << end.DiffUSecs(start)/1000 << "ms)" << endl);
        }
    }

    /*
     * Run the application
     */
    int ret =  application->Run();

    delete application;
    delete data;

    return ret;
}

int PrintAnnotation() {
    Terminal& term = Screen::Instance().MainTerminal();
    int threshold = 25;
    if ( data == nullptr ) {
            COUT ( "Error: annotations are unavailable" << endl;)
    } else {
        string results = data->Annotate(application->ActiveNode(),threshold);
        if ( results == "" ) {
            results = "Error: annotations are unavailable for this node";
        }

        term.Search("![0-9 ][0-9]%!");
        term.PutString(results);
    }
    return 0;
}
