/*
 * GCGVReqFlatView.h
 *
 *  Created on: 6 Apr 2015
 *      Author: lhumphreys
 */

#ifndef GCGVREQFLATVIEW_H_
#define GCGVREQFLATVIEW_H_

#include <SimpleJSON.h>
#include <ReqServer.h>
#include "callCount.h"
#include "GCGVCallgraph.h"


/****************************************************************************
 *                   Return a calculated flat view
 ****************************************************************************/
/*
 * Request the list of files matching a glob pattern.
 *
 * REQUEST:
 *    {
 *        depth: 1,
 *        pageSize: 50,
 *        sortMethod: "Total Time",
 *        filter: "className::.*"
 *    }
 *
 * REPLY:
 *    {
 *        page: 1,
 *        data: [
 *        {
 *            name: "main",
 *            calls: 1,
 *            total: 59234,
 *            average: 59234,
 *        }]
 *    }
 */
class GCGVReqFlatView: public RequestReplyHandler {
public:
    GCGVReqFlatView(GCGV_Callgraph* ptr)
        : parent(ptr) { }

    /**
     * Handle a request for a new
     */
    virtual std::string OnRequest(const char* JSON);

    virtual ~GCGVReqFlatView();
private:
    NewUIntField(depth);
    NewUIntField(pageSize);
    NewStringField(sortMethod);
    NewStringField(filter);
    typedef SimpleParsedJSON<depth,pageSize,sortMethod,filter> Request;

    Request            request;
    GCGV_Callgraph*    parent;
    SimpleJSONBuilder  builder;

    CallCount::SortedCallList callList;

    void SetupRequest(const char* JSON);
    void CalculateCount(CallCount counter);
};

#endif /* GCGVREQFLATVIEW_H_ */
