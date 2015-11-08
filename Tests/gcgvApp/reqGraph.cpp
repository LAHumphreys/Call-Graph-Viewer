#include <tester.h>
#include <SimpleJSON.h>

#include "testReqUtils.h"
#include <GCGVCallgraph.h>

int Depth1(testLogger& log);
int Depth2(testLogger& log);


/**
 * Once the JSON interface supports recursive objects these tests should be
 * updated to use parsed JSON rather than validating the raw string.
 */

int main(int argc, const char *argv[])
{
    Test("Doing a depth1 search...",Depth1).RunTest();
    Test("Doing a depth2 search...",Depth2).RunTest();
    return 0;
}

NewStringField(name);
NewStringField(shortName);
NewI64Field(cost);
typedef SimpleParsedJSON<name,shortName,cost> Graph0;
struct children0: public ObjectArray<Graph0> {
    const char* Name() {
        return "children";
    }
};
typedef SimpleParsedJSON<name,shortName,cost,children0> Graph1;
struct children1: public ObjectArray<Graph1> {
    const char* Name() {
        return "children";
    }
};

typedef SimpleParsedJSON<name,shortName,cost,children1> Graph2;
struct children2: public ObjectArray<Graph2> {
    const char* Name() {
        return "children";
    }
};
typedef SimpleParsedJSON<name,shortName,cost,children2> Graph3;

struct TNode {
    std::string name;
    std::string shortName;
    long cost;
    std::vector<TNode> children;
};

template <size_t i>
struct Children { 
};

template <>
struct Children<0> {
    typedef children2 ChildType;
};

template <>
struct Children<1> {
    typedef children1 ChildType;
};

template <>
struct Children<2> {
    typedef children0 ChildType;
};

template <class RNode>
bool ValidateNodeDetails(testLogger& log, TNode& n, RNode& r) {
    bool ok = true;
    log << "Inspecting node: " << n.name << endl;

    if ( n.name != r.template Get<name>() ) {
        log << "Invalid name for node!" << endl;
        log << ">> " << n.name << endl;
        log << "<< " << r.template Get<name>() << endl;
        ok = false;
    }

    if ( n.shortName != r.template Get<shortName>() ) {
        log << "Invalid shortName for node!" << endl;
        log << ">> " << n.shortName << endl;
        log << "<< " << r.template Get<shortName>() << endl;
        ok = false;
    }

    if ( n.cost != r.template Get<cost>() ) {
        log << "Invalid cost for node!" << endl;
        log << ">> " << n.cost << endl;
        log << "<< " << r.template Get<cost>() << endl;
        ok = false;
    }

    return ok;
}

template <class RNode, size_t depth=0>
bool ValidateNode(testLogger& log, TNode& n, RNode& r) {
    bool ok = ValidateNodeDetails(log,n,r);
    using ChildType = typename Children<depth>::ChildType;

    if ( n.children.size() != r.template Get<ChildType>().size() ) {
        log << "Wrong # of children" << endl;
        log << ">> " << n.children.size() << endl;
        log << "<< " << r.template Get<ChildType>().size() << endl;
        ok = false;
    }

    for (size_t i = 0; ok && i < n.children.size(); ++i) {
        TNode& tchild = n.children[i];
        auto& children = r.template Get<ChildType>();
        auto& child = *children[i];
        using NodeType = decltype(child);
        ok = ValidateNode<NodeType,depth+1>(log,tchild,child);
    }

    return ok;
}

template <>
bool ValidateNode<Graph0&, 3>(testLogger& log, TNode& n, Graph0& r) {
    return ValidateNodeDetails(log,n,r);
}

int Depth1(testLogger& log) {
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
        "depth": 1,
    })JSON"; 

    res = MakeRequest(log,server,"GetGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    Graph3 jsonGraph;
    jsonGraph.Clear();
    std::string error;

    if (!jsonGraph.Parse(res.message.c_str(),error)) {
        log << " Failed to parse result: " << endl;
        log << error << endl;
        return 1;
    }

    auto& ROOT = jsonGraph;

    TNode TROOT = {
        "ROOT",
        "ROOT",
        0,
        {
            {
                "main",
                "main",
                54,
                {}
            }
        }
    };

    if (!ValidateNode<Graph3,0>(log,TROOT,ROOT)) {
        return 1;
    }

    return 0;

}
int Depth2(testLogger& log) {
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
        "depth": 2,
    })JSON"; 

    res = MakeRequest(log,server,"GetGraph",request);

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    Graph3 jsonGraph;
    jsonGraph.Clear();
    std::string error;

    if (!jsonGraph.Parse(res.message.c_str(),error)) {
        log << " Failed to parse result: " << endl;
        log << error << endl;
        return 1;
    }

    auto& ROOT = jsonGraph;

    TNode TROOT = {
        "ROOT",
        "ROOT",
        0,
        {
            {
                "main",
                "main",
                54,
                {
                    {
                        "odds",
                        "odds",
                        75
                    },
                    {
                        "evens",
                        "evens",
                        65
                    }
                }
            }
        }
    };

    if (!ValidateNode<Graph3,0>(log,TROOT,ROOT)) {
        return 1;
    }

    return 0;
}
