/*
 * jsObject.cpp
 *
 *  Created on: 28 Feb 2015
 *      Author: lhumphreys
 */
#include <tester.h>
#include <SimpleJSON.h>

#include "testReqUtils.h"
#include <GCGVCallgraph.h>

NewIntField(page);

NewStringField(name);
NewIntField(calls);
NewI64Field(total);
NewI64Field(average);
typedef SimpleParsedJSON<
   name,
   calls,
   total,
   average
    > Func;
NewObjectArray(data,Func);

struct TFunc {
    std::string name;
    int calls;
    long total;
    long average;
};

typedef SimpleParsedJSON<page,data> FlatView;

int GetFlatView(testLogger& log);
int GetFilteredFlatView(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Validating flat view returns correct data", GetFlatView).RunTest();
    Test("Validating filtered flat view returns correct data", GetFilteredFlatView).RunTest();
    return 0;
}

bool ValidateFunction(Func& f,testLogger& log, TFunc& values) {
    bool ok = true;
    if (f.Get<name>() != values.name ) { 
        log << "Invalid name for " << values.name 
            << ": " << f.Get<name>() << endl;
        ok = false;
    }

    if (f.Get<calls>() != values.calls ) { 
        log << "Invalid value for calls " << values.calls 
            << ": " << f.Get<calls>() << endl;
        ok = false;
    }

    if (f.Get<total>() != values.total ) { 
        log << "Invalid value for total " << values.total 
            << ": " << f.Get<total>() << endl;
        ok = false;
    }

    if (f.Get<average>() != values.average ) { 
        log << "Invalid value for average " << values.average 
            << ": " << f.Get<average>() << endl;
        ok = false;
    }

    return ok;
}

int GetFlatView(testLogger& log) {

    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    std::string request = R"JSON({
        "file": "../data/native/flist_costs.callgrind"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    request = R"JSON({
        "depth": 5,
        "pageSize": 50,
        "sortMethod": "Total Time"
    })JSON"; 

    res = MakeRequest(log,server,"GetFlatView",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }


    FlatView view;
    view.Clear();

    std::string error;
    if (!view.Parse(res.message.c_str(),error)) {
        log << "Failed to parse response: " << endl;
        log << error;
        return 1;
    }

    if (view.Get<page>() != 1 ) {
        log << "Invalid value for page: " << view.Get<page>() << std::endl;
        return 1;
    }


    std::vector<TFunc> expected = {
        {"div",5,87,17},
        {"odds",1,75, 75},
        {"evens",1,65, 65},
        {"main",1,54, 54},
        {"pos_div2",1,7, 7},
        {"pos_div4",1,7, 7}
    };

    auto& rows = view.Get<data>();

    if ( rows.size() != expected.size()) {
        log << "Invalid number of rows: " << rows.size() << endl;
        return 1;
    }

    for (size_t i = 0; i < expected.size(); ++i) {
        if (!ValidateFunction(*rows[i],log,expected[i])) {
            return 1;
        }
    }
    return 0;
}

int GetFilteredFlatView(testLogger& log) {

    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    std::string request = R"JSON({
        "file": "../data/native/flist_costs.callgrind"
    })JSON"; 

    Result res = MakeRequest(log, server,"LoadGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    request = R"JSON({
        "depth": 5,
        "pageSize": 50,
        "sortMethod": "Total Time",
        "filter": ".*d"
    })JSON"; 

    res = MakeRequest(log,server,"GetFlatView",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }


    FlatView view;
    view.Clear();

    std::string error;
    if (!view.Parse(res.message.c_str(),error)) {
        log << "Failed to parse response: " << endl;
        log << error;
        return 1;
    }

    if (view.Get<page>() != 1 ) {
        log << "Invalid value for page: " << view.Get<page>() << std::endl;
        return 1;
    }


    std::vector<TFunc> expected = {
        {"div",5,87,17},
        {"odds",1,75, 75},
        {"pos_div2",1,7, 7},
        {"pos_div4",1,7, 7}
    };

    auto& rows = view.Get<data>();

    if ( rows.size() != expected.size()) {
        log << "Invalid number of rows: " << rows.size() << endl;
        return 1;
    }

    for (size_t i = 0; i < expected.size(); ++i) {
        if (!ValidateFunction(*rows[i],log,expected[i])) {
            return 1;
        }
    }
    return 0;
}
