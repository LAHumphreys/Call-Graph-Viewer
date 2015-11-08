/*
 * GCGVReqChangeNode.h
 *
 *  Created on: 18 Apr 2015
 *      Author: lhumphreys
 */

#ifndef GCGVREQCHANGENODE_H_
#define GCGVREQCHANGENODE_H_

#include "GCGVCallgraph.h"
#include <SimpleJSON.h>
#include <ReqServer.h>

/****************************************************************************
 *                   Return a calculated flat view
 ****************************************************************************/
/*
 * Request the list of files matching a glob pattern.
 *
 * REQUEST:
 *    {
 *        node: "path/to/navigate/to"
 *    }
 *
 * REPLY:
 *    {
 *        pwd: ["ROOT","previous","path","path","to","nagivate","to"]
 *    }
 */
class GCGV_ReqChangeNode: public RequestReplyHandler {
public:
    GCGV_ReqChangeNode(GCGV_Callgraph* ptr)
        : parent(ptr) { }

    virtual std::string OnRequest(const char* JSON);

    virtual ~GCGV_ReqChangeNode();
private:
    NewStringField(node);
    NewStringArrayField(pwd);
    typedef SimpleParsedJSON<node> Request;
    typedef SimpleParsedJSON<pwd>  Reply;

    GCGV_Callgraph*    parent;
    Request request;
    Reply   reply;
};

#endif /* GCGVREQCHANGENODE_H_ */
