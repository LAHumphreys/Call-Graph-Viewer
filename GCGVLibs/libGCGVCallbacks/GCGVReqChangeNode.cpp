/*
 * GCGVReqChangeNode.cpp
 *
 *  Created on: 18 Apr 2015
 *      Author: lhumphreys
 */

#include "GCGVReqChangeNode.h"

std::string GCGV_ReqChangeNode::OnRequest(RequestContext& context) {
    std::string error;
    request.Clear();
    reply.Clear();
    bool ok = true;

    if (!request.Parse(context.request.c_str(), error)) {
        throw CefBaseInvalidRequestException { 0, error };
    }

    Path path(request.Get<node>());

    if (path.Length() == 0) {
        ok = false;
    }

    if(ok && parent->ChangeNode(path)) {
        parent->PWD().GetPath(reply.Get<pwd>());
    } else {
        ok = false;
    }

    if (!ok) {
        throw CefBaseInvalidRequestException {
            0,
            "No such node: " + request.Get<node>()
        };
    }

    return reply.GetJSONString();
}

GCGV_ReqChangeNode::~GCGV_ReqChangeNode() {
    // TODO Auto-generated destructor stub
}

