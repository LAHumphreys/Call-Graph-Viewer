/*
 * jsObject.cpp
 *
 *  Created on: 28 Feb 2015
 *      Author: lhumphreys
 */
#include <tester.h>

#include "testReqUtils.h"
#include <GCGVCallgraph.h>

int LoadGraph_getFileName(testLogger& log);
int LoadGraph_getUnits(testLogger& log);
int LoadGraph_getInitialPath(testLogger& log);
int LoadStack_getFileName(testLogger& log);
int LoadStack_getUnits(testLogger& log);
int LoadStack_getInitialPath(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Validating graph returns correct filename", LoadGraph_getFileName).RunTest();
    Test("Validating graph returns correct units", LoadGraph_getUnits).RunTest();
    Test("Validating graph returns correct initial path", LoadGraph_getInitialPath).RunTest();
    Test("Validating stack returns correct filename", LoadStack_getFileName).RunTest();
    Test("Validating stack returns correct units", LoadStack_getUnits).RunTest();
    Test("Validating stack returns correct initial path", LoadStack_getInitialPath).RunTest();
    return 0;
}

int LoadGraph_getFileName(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    NewStringField(fileName);
    NewStringArrayField(path);
    NewStringArrayField(units);
    SimpleParsedJSON<fileName,path,units> response;

    std::string request = R"JSON({
        "file": "../data/native/flist.callgrind"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    res = MakeRequest(log, server,"GetGraphDetails","{}");

    if (res.error) {
        log << "Unexpected error getting details: " << endl;
        log << res.message << endl;
        return 1;
    }

    if (!Parse(log,response,res.message)) {
        return 1;
    }


    if (response.Get<fileName>() != "flist.callgrind") {
        log << "Unexpected filename: " <<response.Get<fileName>()  << endl;
        return 1;
    }

    return 0;
}

int LoadGraph_getInitialPath(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    NewStringField(fileName);
    NewStringArrayField(path);
    NewStringArrayField(units);
    SimpleParsedJSON<fileName,path,units> response;

    std::string request = R"JSON({
        "file": "../data/native/flist.callgrind"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    res = MakeRequest(log, server,"GetGraphDetails","{}");

    if (res.error) {
        log << "Unexpected error getting details: " << endl;
        log << res.message << endl;
        return 1;
    }

    if (!Parse(log,response,res.message)) {
        return 1;
    }


    if (   response.Get<path>().size() != 1 
        || response.Get<path>()[0] != "ROOT")
    {
        log << "Failed to get correct initial path! " << endl;
        for (const std::string& node: response.Get<path>()) {
            log << "<< " << node << endl;
        }
        return 1;
    }

    return 0;
}

int LoadGraph_getUnits(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    NewStringField(fileName);
    NewStringArrayField(path);
    NewStringArrayField(units);
    SimpleParsedJSON<fileName,path,units> response;

    std::string request = R"JSON({
        "file": "../data/native/flist.callgrind"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    res = MakeRequest(log, server,"GetGraphDetails","{}");

    if (res.error) {
        log << "Unexpected error getting details: " << endl;
        log << res.message << endl;
        return 1;
    }

    if (!Parse(log,response,res.message)) {
        return 1;
    }


    if (   response.Get<units>().size() != 1 
        || response.Get<units>()[0] != "Ir")
    {
        log << "Failed to get correct units! " << endl;
        for (const std::string& unit: response.Get<units>()) {
            log << "<< " << unit << endl;
        }
        return 1;
    }

    return 0;
}

int LoadStack_getFileName(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    NewStringField(fileName);
    NewStringArrayField(path);
    NewStringArrayField(units);
    SimpleParsedJSON<fileName,path,units> response;

    std::string request = R"JSON({
        "file": "../data/exampleGraph.csv"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    res = MakeRequest(log, server,"GetGraphDetails","{}");

    if (res.error) {
        log << "Unexpected error getting details: " << endl;
        log << res.message << endl;
        return 1;
    }

    if (!Parse(log,response,res.message)) {
        return 1;
    }


    if (response.Get<fileName>() != "exampleGraph.csv") {
        log << "Unexpected filename: " <<response.Get<fileName>()  << endl;
        return 1;
    }

    return 0;
}

int LoadStack_getUnits(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    NewStringField(fileName);
    NewStringArrayField(path);
    NewStringArrayField(units);
    SimpleParsedJSON<fileName,path,units> response;

    std::string request = R"JSON({
        "file": "../data/exampleGraph.csv"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    res = MakeRequest(log, server,"GetGraphDetails","{}");

    if (res.error) {
        log << "Unexpected error getting details: " << endl;
        log << res.message << endl;
        return 1;
    }

    if (!Parse(log,response,res.message)) {
        return 1;
    }

    if (   response.Get<units>().size() != 1 
        || response.Get<units>()[0] != "us")
    {
        log << "Failed to get correct units! " << endl;
        for (const std::string& unit: response.Get<units>()) {
            log << "<< " << unit << endl;
        }
        return 1;
    }

    return 0;
}

int LoadStack_getInitialPath(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    NewStringField(fileName);
    NewStringArrayField(path);
    NewStringArrayField(units);
    SimpleParsedJSON<fileName,path,units> response;

    std::string request = R"JSON({
        "file": "../data/exampleGraph.csv"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    res = MakeRequest(log, server,"GetGraphDetails","{}");

    if (res.error) {
        log << "Unexpected error getting details: " << endl;
        log << res.message << endl;
        return 1;
    }

    if (!Parse(log,response,res.message)) {
        return 1;
    }

    if (   response.Get<path>().size() != 1 
        || response.Get<path>()[0] != "ROOT")
    {
        log << "Failed to get correct initial path! " << endl;
        for (const std::string& node: response.Get<path>()) {
            log << "<< " << node << endl;
        }
        return 1;
    }

    return 0;
}
