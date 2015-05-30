/*
 * jsObject.cpp
 *
 *  Created on: 28 Feb 2015
 *      Author: lhumphreys
 */
#include <env.h>
#include <iostream>

#include <CefTestApp.h>
#include <CefTests.h>

#include "GCGVBrowser_App.h"
#include <GCGV_Callbacks.h>
#include "CefBaseMainLoop.h"

int Files(CefTestContext& context);
int LoadGraph(CefTestContext& context);
int GetFileName(CefTestContext& context);
int GetUnits(CefTestContext& context);
int FlatView(CefTestContext& context);
int FilteredFlatView(CefTestContext& context);
int ChangeNode(CefTestContext& context);
int FindNodes(CefTestContext& context);
int GetInitialPath(CefTestContext& context);

int main(int argc, char **argv) {

    cout.sync_with_stdio();

    CefBaseApp& app = DummyCefApp::Instance();

    GCGV_Callbacks::InstallNewHandlers(app.Client(),true);

    DummyCefApp::Instance().AddTest(
        "Sending a file request",
        Files);

    DummyCefApp::Instance().AddTest(
        "Open callgrind files.",
        LoadGraph);

    DummyCefApp::Instance().AddTest(
        "Get the name of the opened file.",
        GetFileName);

    DummyCefApp::Instance().AddTest(
        "Get the initial path once a file has been opened.",
        GetInitialPath);

    DummyCefApp::Instance().AddTest(
        "Get the units used by the file",
        GetUnits);

    DummyCefApp::Instance().AddTest(
        "Flat view",
        FlatView);

    DummyCefApp::Instance().AddTest(
        "Filtered Flat view",
        FilteredFlatView);

    DummyCefApp::Instance().AddTest(
        "Change to node",
        ChangeNode);

    DummyCefApp::Instance().AddTest(
        "Find nodes",
        FindNodes);

    DummyCefApp::Instance().RunTestsAndExit(argc, argv);

    cout << "ERROR: Run and exit should have exited!: " << endl;
    return 1;
}

int Files(CefTestContext& context) {
    context.Log() << "Sending request..." << endl;
    context.ExecuteCleanJS(R"JSCODE(
        var result="READY";

        function Success(response) {
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            var obj = JSON.parse(response);
            result = obj.files[0];
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        DEV_TOOLS_Test_Log("Sending ping request");


        result;
    )JSCODE" , "READY");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {
            pattern: 1
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVFileList " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "FAIL: Invalid type for field: pattern");
        return true;
    },1e6, "Waiting for the framework to reject our ls request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVFileList " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "FAIL: No pattern provided!");
        return true;
    },1e6, "Waiting for the framework to reject our ls request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {
            pattern: "*.cpp",
            prefix: "DEV_TOOLS/"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVFileList " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "DEV_TOOLS/gcgv_jsTests.cpp");
        return true;
    },1e6, "Waiting for the framework to reject our ls request");

    return 0;
}

int LoadGraph(CefTestContext& context) {

    context.Log() << "Sending request..." << endl;
    context.ExecuteCleanJS(R"JSCODE(
        var result="READY";

        function Success(response) {
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            result = "OPENED";
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        DEV_TOOLS_Test_Log("Sending ping request");


        result;
    )JSCODE" , "READY");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {
            file: "THIS IS NOT A FILE!"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVLoadGraph " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "FAIL: THIS IS NOT A FILE! is not a valid call-grind file.");
        return true;
    },1e6, "Waiting for the framework to reject our ls request",5e5);

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {
            file: "../data/native/flist.callgrind"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVLoadGraph " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED");
        return true;
    },1e6, "Waiting for the framework to reject our ls request");

    return 0;

}

int GetFileName(CefTestContext& context) {

    context.Log() << "Sending request..." << endl;
    context.ExecuteCleanJS(R"JSCODE(
        var result="READY";

        function Success(response) {
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            result = "OPENED";
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        DEV_TOOLS_Test_Log("Sending ping request");


        result;
    )JSCODE" , "READY");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {
            file: "../data/native/flist.callgrind"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVLoadGraph " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {};


        window.gcgvQuery({
            request: "REQUEST_GCGVGetGraphDetails " + JSON.stringify(request),
            onSuccess: function (resp) {
                var response = JSON.parse(resp);
                result = response.fileName;
            },
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "flist.callgrind");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    return 0;

}

int GetInitialPath(CefTestContext& context) {

    context.Log() << "Sending request..." << endl;
    context.ExecuteCleanJS(R"JSCODE(
        var result="READY";

        function Success(response) {
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            result = "OPENED";
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        result;
    )JSCODE" , "READY");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {
            file: "../data/native/flist.callgrind"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVLoadGraph " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {};


        window.gcgvQuery({
            request: "REQUEST_GCGVGetGraphDetails " + JSON.stringify(request),
            onSuccess: function (resp) {
                var response = JSON.parse(resp);
                result = response.path.toString();
            },
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "ROOT");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    return 0;

}

int GetUnits(CefTestContext& context) {

    context.Log() << "Sending request..." << endl;
    context.ExecuteCleanJS(R"JSCODE(
        var result="READY";

        function Success(response) {
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            result = "OPENED";
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        DEV_TOOLS_Test_Log("Sending ping request");


        result;
    )JSCODE" , "READY");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {
            file: "../data/native/flist.callgrind"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVLoadGraph " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        var request = {};


        window.gcgvQuery({
            request: "REQUEST_GCGVGetGraphDetails " + JSON.stringify(request),
            onSuccess: function (resp) {
                var i, response = JSON.parse(resp);
                result = "";
                for ( i = 0; i < response.units.length; i += 1) {
                    result += response.units[i];
                }
            },
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "Ir");
        return true;
    },1e6, "Waiting for the framework to handle our details request");

    return 0;

}

int FlatView(CefTestContext& context) {
    std::string result = "div: 5,87,17\n";
    result += "odds: 1,75,75\n";
    result += "evens: 1,65,65\n";

    context.ExecuteCleanJS(R"JSCODE(
        var result="RUNNING";

        function Success(response) {
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            result = "OPENED";
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        var request = {
            file: "../data/native/flist_costs.callgrind"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVLoadGraph " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        function Success(response) { 
            DEV_TOOLS_Test_Log( "Got response: " + response);

            result = "";
            var data, i;
            data = JSON.parse(response);


            for (i = 0; i < data.data.length; i += 1) {
                result += data.data[i].name + ": " 
                              + data.data[i].calls + "," 
                              + data.data[i].total + ","
                              + data.data[i].average + "\n";
            }
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        request = {
            depth: 5,
            pageSize: 3,
            sortMethod: "Total Time"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVGetFlatView " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , result);
        return true;
    },1e6, "Waiting for call-graph data...");
    return 0;
}

int FilteredFlatView(CefTestContext& context) {
    std::string result = "div: 5,87,17\n";
    result += "odds: 1,75,75\n";
    result += "pos_div2: 1,7,7\n";

    context.ExecuteCleanJS(R"JSCODE(
        var result="RUNNING";

        function Success(response) {
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            result = "OPENED";
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        var request = {
            file: "../data/native/flist_costs.callgrind"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVLoadGraph " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        function Success(response) { 
            DEV_TOOLS_Test_Log( "Got response: " + response);

            result = "";
            var data, i;
            data = JSON.parse(response);


            for (i = 0; i < data.data.length; i += 1) {
                result += data.data[i].name + ": " 
                              + data.data[i].calls + "," 
                              + data.data[i].total + ","
                              + data.data[i].average + "\n";
            }
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        request = {
            depth: 5,
            pageSize: 3,
            sortMethod: "Total Time",
            filter: ".*d"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVGetFlatView " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , result);
        return true;
    },1e6, "Waiting for call-graph data...");
    return 0;
}


int ChangeNode(CefTestContext& context) {
    context.ExecuteCleanJS(R"JSCODE(
        var result="RUNNING";

        function Success(response) {
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            result = "OPENED";
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        var request = {
            file: "../data/native/flist_costs.callgrind"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVLoadGraph " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        var result="RUNNING";

        function Success(response) {
            var data;
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            data = JSON.parse(response);
            result = "OPENED: " + data.pwd.join("/");
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        var request = {
            node: "main/evens"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVChangeNode " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED: ROOT/main/evens");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        request = {
            node: "div"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVChangeNode " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED: ROOT/main/evens/div");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        request = {
            node: "../../odds"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVChangeNode " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED: ROOT/main/odds");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        request = {
            node: "ROOT/main"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVChangeNode " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED: ROOT/main");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        request = {
            node: "NOT_A_NODE"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVChangeNode " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "FAIL: No such node: NOT_A_NODE");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        request = {
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVChangeNode " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "FAIL: No such node: ");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        request = {
            node: "ROOT/main/evens/div"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVChangeNode " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED: ROOT/main/evens/div");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

   return 0;
}

int FindNodes(CefTestContext& context) {
    context.ExecuteCleanJS(R"JSCODE(
        var result="RUNNING";

        function Success(response) {
            DEV_TOOLS_Test_Log(">>> Request handled successfully!");
            result = "OPENED";
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        var request = {
            file: "../data/native/flist_costs.callgrind"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVLoadGraph " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });

        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "OPENED");
        return true;
    },1e6, "Waiting for the framework to handle our load request");

    context.ExecuteCleanJS(R"JSCODE(
        var result="RUNNING", nodes = [];

        function Success(response) {
            var data;
            DEV_TOOLS_Test_Log(">>> Request handled successfully: " + response);
            data = JSON.parse(response);
            result = "FOUND: " + data.nodes.length;
            nodes = data.nodes;
        }

        function Failure(err_code, err_message) {
            DEV_TOOLS_Test_Log(
                ">>> Failed to handle query: " + err_message + "(" + err_code + ")");
            result = "FAIL: " + err_message;
        }

        var request = {
            name: "div",
            depth: 1,
            sortBy: "cost"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVFindNodes " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "FOUND: 0");
        return true;
    },1e6, "Waiting for the framework to handle our find request");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        request = {
            name: "div",
            depth: 3,
            sortBy: "cost"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVFindNodes " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "FOUND: 2");
        return true;
    },1e6, "Waiting for the framework to find our div nodes");

    context.ExecuteCleanJS("nodes[0].path", "main/odds/div");
    context.ExecuteCleanJS("nodes[0].calls.toString()", "3");
    context.ExecuteCleanJS("nodes[0].cost.toString()", "45");
    context.ExecuteCleanJS("nodes[0].avCost.toString()", "15");
    context.ExecuteCleanJS("nodes[1].path", "main/evens/div");
    context.ExecuteCleanJS("nodes[1].calls.toString()", "2");
    context.ExecuteCleanJS("nodes[1].cost.toString()", "42");
    context.ExecuteCleanJS("nodes[1].avCost.toString()", "21");

    context.ExecuteCleanJS(R"JSCODE(
        result="RUNNING";

        request = {
            name: "div",
            depth: 3,
            sortBy: "avCost"
        };

        window.gcgvQuery({
            request: "REQUEST_GCGVFindNodes " + JSON.stringify(request),
            onSuccess: Success,
            onFailure: Failure });
        result;
    )JSCODE" , "RUNNING");

    context.WaitForResult([&] () -> bool {
        context.ExecuteCleanJS(R"JSCODE(
            result;
        )JSCODE" , "FOUND: 2");
        return true;
    },1e6, "Waiting for the framework to find our div nodes");

    context.ExecuteCleanJS("nodes[0].path", "main/evens/div");
    context.ExecuteCleanJS("nodes[0].calls.toString()", "2");
    context.ExecuteCleanJS("nodes[0].cost.toString()", "42");
    context.ExecuteCleanJS("nodes[0].avCost.toString()", "21");
    context.ExecuteCleanJS("nodes[1].path", "main/odds/div");
    context.ExecuteCleanJS("nodes[1].calls.toString()", "3");
    context.ExecuteCleanJS("nodes[1].cost.toString()", "45");
    context.ExecuteCleanJS("nodes[1].avCost.toString()", "15");

    return 0;
}
