/*
 * GCGVReqGraphDetails.h
 *  Created on: 6 Apr 2015
 *      Author: lhumphreys
 */

#ifndef GCGVREQGRAPHDETAILS_H_
#define GCGVREQGRAPHDETAILS_H_

#include "GCGV_Callbacks.h"
#include <SimpleJSON.h>
#include <CefBaseRequestReplies.h>


/****************************************************************************
 *                   Return details of currently loaded graph
 ****************************************************************************/
/*
 * Request the list of files matching a glob pattern.
 *
 * REQUEST:
 *    {
 *    }
 *
 * REPLY:
 *    {
 *        fileName:  "gather.out",
 *        units: ["Ir", "Dr"],
 *        path: ["ROOT"]
 *    }
 */
class GCGVCallgraph_GetGraphDetails: public CefBaseJSRequestReply {
public:
    GCGVCallgraph_GetGraphDetails(GCGV_Callgraph* ptr)
        : parent(ptr) { }

    virtual std::string OnRequest(RequestContext& context);

    virtual ~GCGVCallgraph_GetGraphDetails() { }
private:

    void LookupUnits();
    NewStringField(fileName);
    NewStringArrayField(units);
    NewStringArrayField(path);
    typedef SimpleParsedJSON<fileName,units,path> Reply;

    Reply   reply;

    GCGV_Callgraph*  parent;
};


#endif /* GCGVREQGRAPHDETAILS_H_ */
