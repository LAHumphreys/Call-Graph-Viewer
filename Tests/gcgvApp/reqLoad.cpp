/*
 * jsObject.cpp
 *
 *  Created on: 28 Feb 2015
 *      Author: lhumphreys
 */
#include <tester.h>

#include "testReqUtils.h"
#include <GCGVCallgraph.h>

int LoadGraph_ValidFile(testLogger& log);
int LoadGraph_loadFile(testLogger& log);
int LoadStack_InvalidCSV(testLogger& log);
int LoadStack_loadFile(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Validating graph load validates file", LoadGraph_ValidFile).RunTest();
    Test("Validating graph load handles a valid graph", LoadGraph_loadFile).RunTest();
    Test("Validating stack handles invalid CSV", LoadStack_InvalidCSV).RunTest();
    Test("Validating stack load handles a valid CSV", LoadStack_loadFile).RunTest();
    return 0;
}

int LoadGraph_ValidFile(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    std::string request = R"JSON({
        "file": "THIS IS NOT A FILE!"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    std::string expErr = "THIS IS NOT A FILE! is not a valid call-grind file.";

    if (!ValidateError(log,res,0,expErr)) {
        return 1;
    }

    return 0;
}

int LoadGraph_loadFile(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    std::string request = R"JSON({
        "file": "../data/native/flist.callgrind"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    return 0;
}


int LoadStack_InvalidCSV(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    std::string request = R"JSON({
        "file": "../data/basicInput.csv"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    std::string expErr = "../data/basicInput.csv is not a valid call-grind file.";

    if (!ValidateError(log,res,0,expErr)) {
        return 1;
    }

    return 0;
}

int LoadStack_loadFile(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    std::string request = R"JSON({
        "file": "../data/exampleGraph.csv"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    return 0;
}


