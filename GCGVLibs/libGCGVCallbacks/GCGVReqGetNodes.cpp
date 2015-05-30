/*
 * GCGVReqGetNodes.cpp
 *
 *  Created on: 18 Apr 2015
 *      Author: lhumphreys
 */

#include "GCGVReqGetNodes.h"
#include <nodeSearchCache.h>

GCGV_ReqGetNodes::~GCGV_ReqGetNodes() {
    // TODO Auto-generated destructor stub
}

std::string GCGV_ReqGetNodes::OnRequest(RequestContext& context) {
    SetupRequest(context);

    FindNodes();

    BuildReply();

    return reply.GetAndClear();
}

void GCGV_ReqGetNodes::SetupRequest(RequestContext& context) {
    std::string error;
    request.Clear();
    reply.Clear();
    sortedNodes.clear();
    sort = COST;

    if (!request.Parse(context.request.c_str(), error)) {
        throw CefBaseInvalidRequestException { 0, error };
    }
    if (request.Get<sortBy>() == "cost") {
        sort = COST;
    } else if (request.Get<sortBy>() == "avCost") {
        sort = AV_COST;
    } else {
        error = "Invalid sort type: " + request.Get<sortBy>();
        throw CefBaseInvalidRequestException { 0, error };
    }
}

void GCGV_ReqGetNodes::FindNodes() {
    SearchCache cache;
    NodePtr node = parent->CWD();
    const std::string& nameVal = request.Get<name>();
    const int depthVal = request.Get<depth>();

    sortedNodes.AddTree(node,sort,nameVal,depthVal);
}

void GCGV_ReqGetNodes::BuildReply() {
    size_t pwdLen = parent->PWD().ToString().length() +1;
    reply.StartArray("nodes");
        for (SortedNodes::reverse_iterator it = sortedNodes.rbegin();
             it != sortedNodes.rend();
             ++it)
        {
            NodePtr node = it->second;

            long total = node->Costs()[0];
            long average = double(total) / node->Calls();
            std::string path =
                    GCGV_Callgraph::GetPath(node).ToString();

            if (path.length() > pwdLen) {
                path = path.c_str() + pwdLen;
            }

            reply.StartAnonymousObject();
                reply.Add("path", path);
                reply.Add("calls", node->Calls());
                reply.Add("cost", total);
                reply.Add("avCost", average);
            reply.EndObject();
        }
    reply.EndArray();
}

void GCGV_ReqGetNodes::SortedNodes::AddTree(
    NodePtr root,
    SORT_BY sort,
    const std::string& filter,
    int depth)
{
    if (root->Name() == filter) {
        AddNode(root,sort);
    }

    --depth;
    if (depth != -1) {
        root->ForEach([=] (NodePtr&& node) -> void {
            this->AddTree(node,sort,filter,depth);
        });
    }
}

void GCGV_ReqGetNodes::SortedNodes::AddNode(NodePtr node, SORT_BY sort) {
    long idx = 0;

    switch (sort) {
        case COST:
            idx = node->Costs()[0];
            break;
        case AV_COST:
            idx = node->Costs()[0] / node->Calls();
            break;
        default:
            throw CefBaseInvalidRequestException { 0, "Invalid Sort Type" };
    }

    insert(SortedNodes::value_type(idx, node));
}
