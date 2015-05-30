/*
 * GCGVReqChangeNode.h
 *
 *  Created on: 18 Apr 2015
 *      Author: lhumphreys
 */

#ifndef GCGVREQCHANGENODE_H_
#define GCGVREQCHANGENODE_H_

#include "GCGV_Callbacks.h"
#include <SimpleJSON.h>
#include <CefBaseRequestReplies.h>

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
class GCGV_ReqChangeNode: public CefBaseJSRequestReply {
public:
    GCGV_ReqChangeNode(GCGV_Callgraph* ptr)
        : parent(ptr) { }

    virtual std::string OnRequest(RequestContext& context);

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
