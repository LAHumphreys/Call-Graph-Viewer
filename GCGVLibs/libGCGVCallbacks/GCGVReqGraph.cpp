/*
 * GCGVCallgraphGetGraph.cpp
 *
 *  Created on: 31 May 2015
 *      Author: lhumphreys
 */

#include "GCGVReqGraph.h"

std::string GCGVCallgraph_ReqGraph::OnRequest(RequestContext& context) {
    std::string error;
    request.Clear();
    builder.Clear();

    if ( !request.Parse(context.request.c_str(),error)) {
        throw CefBaseInvalidRequestException{0,error};
    }

    if (parent->CWD().IsNull()) {
        throw CefBaseInvalidRequestException {
            0,
            "No callgraph has been loaded!" };
    }

    AddNode(parent->CWD(),request.Get<depth>());

    return builder.GetAndClear();
}

GCGVCallgraph_ReqGraph::~GCGVCallgraph_ReqGraph() {
}

void GCGVCallgraph_ReqGraph::AddNode(NodePtr node, size_t node_depth) {
    if (node_depth == request.Get<depth>()) {
       // The top-level node lives in the top-level JSON, so does not need an
       // object wrapper
    } else {
        builder.StartAnonymousObject();
    }

    builder.Add("name",node->Name());
    if (node->Name().length() > 50) {
        builder.Add("shortName",node->Name().substr(0,50));
    } else  {
        builder.Add("shortName",node->Name());
    }
    builder.Add("cost",node->Costs()[0]);

    if (node_depth>0 && node->NumChildren() > 0)
    {
        builder.StartArray("children");
        node-> ForEach([&] (NodePtr child) -> void {
            this->AddNode(child,node_depth-1);
        });
        builder.EndArray();

    }

    if (node_depth == request.Get<depth>()) {
       // The top-level node lives in the top-level JSON, so does not need an
       // object wrapper
    } else {
        builder.EndObject();
    }

}
