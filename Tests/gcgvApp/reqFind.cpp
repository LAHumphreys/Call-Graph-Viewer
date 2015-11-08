#include <tester.h>
#include <SimpleJSON.h>

#include "testReqUtils.h"
#include <GCGVCallgraph.h>

int FindExact(testLogger& log);
int AvCostSort(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Validating exact find...", FindExact).RunTest();
    Test("Sorting by average cost...", AvCostSort).RunTest();
    return 0;
}

NewStringField(path);
NewIntField(calls);
NewI64Field(cost);
NewI64Field(avCost);
typedef SimpleParsedJSON<
   path,
   calls,
   cost,
   avCost
    > Func;
NewObjectArray(nodes,Func);

typedef SimpleParsedJSON<nodes> Response;

struct TFunc {
    std::string path;
    int calls;
    long cost;
    long avCost;
};

bool ValidateFunction(Func& f,testLogger& log, TFunc& values) {
    bool ok = true;
    if (f.Get<path>() != values.path ) { 
        log << "Invalid path for " << values.path 
            << ": " << f.Get<path>() << endl;
        ok = false;
    }

    if (f.Get<calls>() != values.calls ) { 
        log << "Invalid value for calls " << values.calls 
            << ": " << f.Get<calls>() << endl;
        ok = false;
    }

    if (f.Get<cost>() != values.cost ) { 
        log << "Invalid value for cost " << values.cost 
            << ": " << f.Get<cost>() << endl;
        ok = false;
    }

    if (f.Get<avCost>() != values.avCost ) { 
        log << "Invalid value for avCost " << values.avCost 
            << ": " << f.Get<avCost>() << endl;
        ok = false;
    }

    return ok;
}

int FindExact(testLogger& log) {

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
        "name": "div",
        "depth": 3,
        "sortBy": "cost"
    })JSON"; 

    res = MakeRequest(log,server,"FindNodes",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    Response json;
    json.Clear();

    std::string error;
    if (!json.Parse(res.message.c_str(),error)) {
        log << "Failed to parse response: " << endl;
        log << error;
        return 1;
    }

    std::vector<TFunc> expected = {
        {"main/odds/div",3,45,15},
        {"main/evens/div",2,42,21}
    };

    auto& rows = json.Get<nodes>();

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

int AvCostSort(testLogger& log) {

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
        "name": "div",
        "depth": 3,
        "sortBy": "avCost"
    })JSON"; 

    res = MakeRequest(log,server,"FindNodes",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    Response json;
    json.Clear();

    std::string error;
    if (!json.Parse(res.message.c_str(),error)) {
        log << "Failed to parse response: " << endl;
        log << error;
        return 1;
    }

    std::vector<TFunc> expected = {
        {"main/evens/div",2,42,21},
        {"main/odds/div",3,45,15}
    };

    auto& rows = json.Get<nodes>();

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
