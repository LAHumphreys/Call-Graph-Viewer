/*
 * GCGVReqChangeNode.cpp
 *
 *  Created on: 18 Apr 2015
 *      Author: lhumphreys
 */

#include "GCGVReqChangeNode.h"

std::string GCGV_ReqChangeNode::OnRequest(const char* JSON) {
    std::string error;
    request.Clear();
    reply.Clear();
    bool ok = true;

    if (!request.Parse(JSON, error)) {
        throw InvalidRequestException { 0, error };
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
        throw InvalidRequestException {
            0,
            "No such node: " + request.Get<node>()
        };
    }

    return reply.GetJSONString();
}

GCGV_ReqChangeNode::~GCGV_ReqChangeNode() {
}

