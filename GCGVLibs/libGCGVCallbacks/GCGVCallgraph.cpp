/*
 * GCGVCallgraph.cpp
 *
 *  Created on: 6 Apr 2015
 *      Author: lhumphreys
 */

#include "GCGVCallgraph.h"
#include <SimpleJSON.h>
#include <CefBaseRequestReplies.h>
#include <nodeConfig.h>
#include <OSTools.h>
#include "GCGVReqGraphDetails.h"
#include <GCGVReqFlatView.h>
#include <GCGVReqChangeNode.h>
#include <GCGVReqGetNodes.h>
#include <GCGVReqGraph.h>
#include <util_time.h>
#include <stack.h>
#include <csv.h>
#include <stdReader.h>

/****************************************************************************
 *                      Load a new callgrind file
 ****************************************************************************/

/*
 * Request the list of files matching a glob pattern.
 *
 * REQUEST:
 *    {
 *         file: "gather.out",
 *    }
 *
 * REPLY:
 *    {
 *    }
 */
class GCGVCallgraph_LoadNewFile: public CefBaseJSRequestReply {
public:
    GCGVCallgraph_LoadNewFile(GCGV_Callgraph* ptr)
        : parent(ptr) { }

    virtual std::string OnRequest(RequestContext& context) {
        request.Clear();

        static std::string error;

        if ( !request.Parse(context.request.c_str(),error)) {
            throw CefBaseInvalidRequestException{0,error};
        }

        if ( !parent->LoadGraph(request.Get<file>()))
        {
            error = request.Get<file>()  + " is not a valid call-grind file.";

            throw CefBaseInvalidRequestException{0,error};
        }

        return reply.GetJSONString();
    }

    virtual ~GCGVCallgraph_LoadNewFile() { }
private:
    NewStringField(file);
    typedef SimpleParsedJSON<file> Request;
    typedef SimpleParsedJSON<> Reply;

    Request request;
    Reply   reply;

    GCGV_Callgraph*  parent;
};


/****************************************************************************
 *                      Callgraph implementation
 ****************************************************************************/

GCGV_Callgraph::GCGV_Callgraph()
    :cwd(nullptr),
     root(nullptr)
{
}

GCGV_Callgraph::~GCGV_Callgraph() {
}

void GCGV_Callgraph::InstallHandlers(CefBaseJSRequestReplyHandler& ReqReps) {
    std::shared_ptr<GCGVCallgraph_LoadNewFile> loadHandler(
        new GCGVCallgraph_LoadNewFile(this));

    ReqReps.InstallHandler("GCGVLoadGraph",loadHandler);

    std::shared_ptr<GCGVCallgraph_GetGraphDetails> detailsHandler(
        new GCGVCallgraph_GetGraphDetails(this));

    ReqReps.InstallHandler("GCGVGetGraphDetails",detailsHandler);

    std::shared_ptr<GCGVReqFlatView> flatHandler(
        new GCGVReqFlatView(this));

    ReqReps.InstallHandler("GCGVGetFlatView",flatHandler);

    std::shared_ptr<GCGV_ReqChangeNode> cdHandler (
        new GCGV_ReqChangeNode(this));

    ReqReps.InstallHandler("GCGVChangeNode",cdHandler);

    std::shared_ptr<GCGV_ReqGetNodes> getNodesHandler (
        new GCGV_ReqGetNodes(this));

    ReqReps.InstallHandler("GCGVFindNodes",getNodesHandler);

    std::shared_ptr<GCGVCallgraph_ReqGraph> getGraphHandler (
        new GCGVCallgraph_ReqGraph(this));

    ReqReps.InstallHandler("GCGVGetGraph",getGraphHandler);
}

bool GCGV_Callgraph::LoadGraph(const std::string& fname) {
    bool ok = false;

    size_t len = fname.length();
    size_t extension = len - 4;
    if ( len > 4 && fname.substr(extension,len) == ".csv") {
        ok = store.LoadStack(fname);
    } else {
        ok = store.LoadGraph(fname);
    }

    if (ok)
    {
        LoadGraph(fname,store.RootNode());
    }


    return ok;
}

bool GCGV_Callgraph::LoadGraph(const std::string& fname, NodePtr rootNode) {

    this->fname = OS::Basename(fname);
    root = rootNode;
    cwd = rootNode;

    return true;
}

Path GCGV_Callgraph::PWD() const {
    return GetPath(cwd);
}

Path GCGV_Callgraph::GetPath(NodePtr node) {
    static std::vector<std::string> path;
    path.clear();
    for ( NodePtr i = node; !i.IsNull(); i = i->Parent() ) {
        path.push_back(i->Name());
    }
    return Path(path.rbegin(),path.rend());
}

bool GCGV_Callgraph::ChangeNode(const Path& path) {
    bool changed = false;
    NodePtr node = nullptr;

    if (path.Root().Name() == "ROOT") {
        node = root->GetNode(path.Root().Next());
    } else {
        node = cwd->GetNode(path);
    }

    if (!node.IsNull()) {
        cwd = node;
        changed = true;
    }

    return changed;
}

GCGV_Callgraph::GraphStorage::GraphStorage()
   : root(nullptr),
     graph(nullptr)
{
}


bool GCGV_Callgraph::GraphStorage::LoadStack(const std::string& fname) {
    bool ok = true;

    NodeConfig::Instance().Reset();
    NodeConfig::Instance().ConfigureCostFactory("us");
    root = new Node();

    CallStack stack(root);

    struct Direction {
        struct ParseError { };
        Direction (const std::string& token) {
            if (token == "ENTER") {
                entry = true;
            } else if (token == "LEAVE") {
                entry = false;
            } else {
                throw ParseError();
            }
        }
        bool entry;
    };
    long dummy = 0;
    typedef CSV<Time,Direction,std::string> DataFile;
    const size_t TIME = 0;
    const size_t DIRECTION = 1;
    const size_t NAME = 2;

    IFStreamReader reader(fname.c_str());
    try {
        DataFile calls(DataFile::LoadCSV(reader));

        for (int i = 0; i < calls.Rows(); ++i) {
            const std::string& name = calls.GetCell<NAME>(i);
            const Time& eventTime = calls.GetCell<TIME>(i);
            const bool& newFrame = calls.GetCell<DIRECTION>(i).entry;

            if (newFrame) {
                stack.AddFrame(name,eventTime);
            } else {
                stack.LeaveFrame(name,eventTime,dummy,true);
            }
        }
    } catch (Direction::ParseError& error) {
        ok = false;
    }

    return ok;
}

bool GCGV_Callgraph::GraphStorage::LoadGraph(const std::string& fname) {
    // TODO: Fix this singleton pattern!
    bool ok = true;
    NodeConfig::Instance().Reset();

    graph.reset(new CallgrindNative(fname));

    ok = !graph->RootNode().IsNull() && graph->RootNode()->NumChildren() > 0;

    if (ok) {
        root = graph->RootNode();
    } else {
        graph.reset(nullptr);
    }

    return ok;
}

NodePtr GCGV_Callgraph::GraphStorage::RootNode() {
    return root;
}
