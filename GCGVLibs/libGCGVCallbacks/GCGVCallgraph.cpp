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
    :cwd(nullptr)
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
}

bool GCGV_Callgraph::LoadGraph(const std::string& fname) {
    bool ok = false;

    // TODO: Fix this singleton pattern!
    NodeConfig::Instance().Reset();
    graph.reset(new CallgrindNative(fname));

    ok = !graph->RootNode().IsNull() && graph->RootNode()->NumChildren() > 0;

    if (ok) {
        this->fname = OS::Basename(fname);
        cwd = graph->RootNode();
    } else {
        graph.reset(nullptr);
    }

    return ok;
}
