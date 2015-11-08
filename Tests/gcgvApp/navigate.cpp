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

NewStringArrayField(pwd);
typedef SimpleParsedJSON<pwd> Response;

int CDAbsolute(testLogger& log);
int CDRelative(testLogger& log);
int CDNoSuchNode(testLogger& log);

int main(int argc, const char *argv[])
{
    Test("Changing to a relative node...",CDRelative).RunTest();
    Test("Changing to an absolute node...",CDAbsolute).RunTest();
    Test("Changing to an invalid node...",CDNoSuchNode).RunTest();
    return 0;
}

struct Request {
    std::string                  file;
    std::vector<std::string>     reqPaths;
    std::vector<
        std::vector<std::string>
    >                            paths;
};

int DoCDRequest(testLogger& log, Request& r) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    NewStringField(file);
    SimpleParsedJSON<file> openRequest;
    openRequest.Clear();
    openRequest.Get<file>() = r.file;

    Result res = MakeRequest(log, server,"LoadGraph",openRequest.GetJSONString());

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    NewStringField(node);
    SimpleParsedJSON<node> cdRequest;

    if ( r.paths.size() != r.reqPaths.size()) {
        log << "Req / result paths num missmatch!" << endl;
        return 2;
    }

    for ( size_t i = 0; i < r.paths.size(); ++i ) {
        std::vector<std::string>& path = r.paths[i];

        cdRequest.Clear();
        cdRequest.Get<node>() = r.reqPaths[i];

        res = MakeRequest(log,server,"ChangeNode",cdRequest.GetJSONString());

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
            log << error << endl;
            return 1;
        }

        auto& endPath = json.Get<pwd>();

        if ( path.size() != endPath.size()) {
            log << "Invalid number of path nodes: " << endPath.size() << endl;
            return 1;
        }

        for (size_t i = 0; i < path.size(); ++i) {
            if (path[i] != endPath[i]) {
                log << "Node miss-match at " << i << endl;
                log << ">> " << path[i] << endl;
                log << "<< " << endPath[i] << endl;
                return 1;
            }
        }
    }

    return 0;
}

int CDAbsolute(testLogger& log) {

    Request request {
        "../data/native/flist_costs.callgrind",
        {
            "main/evens",
            "ROOT/main"
        },
        {
            {
                "ROOT",
                "main",
                "evens"
            },
            {
                "ROOT",
                "main"
            }
        }
    };

    return DoCDRequest(log,request);
}

int CDRelative(testLogger& log) {

    Request request {
        "../data/native/flist_costs.callgrind",
        {
            "main/evens",
            "div",
            "../../odds"
        },
        {
            {
                "ROOT",
                "main",
                "evens"
            },
            {
                "ROOT",
                "main",
                "evens",
                "div",
            },
            {
                "ROOT",
                "main",
                "odds"
            }
        }
    };

    return DoCDRequest(log,request);
}

int CDNoSuchNode(testLogger& log) {
    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    NewStringField(file);
    SimpleParsedJSON<file> openRequest;
    openRequest.Clear();
    openRequest.Get<file>() = "../data/native/flist_costs.callgrind";

    Result res = MakeRequest(log, server,"LoadGraph",openRequest.GetJSONString());

    if (res.error) {
        log << "Unexpected error: " << endl;
        log << res.message << endl;
        return 1;
    }

    res = MakeRequest(log, server,"ChangeNode","{\"node\":\"NOT_A_NODE\"}");

    if (!ValidateError(log,res,0,"No such node: NOT_A_NODE")) {
        return 1;
    }

    res = MakeRequest(log, server,"ChangeNode","{}");

    if (!ValidateError(log,res,0,"No such node: ")) {
        return 1;
    }

    return 0;
}
