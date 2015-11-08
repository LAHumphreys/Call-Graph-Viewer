/*
 * GCGVReqGetNodes.h
 *
 *  Created on: 18 Apr 2015
 *      Author: lhumphreys
 */

#ifndef GCGVREQGETNODES_H_
#define GCGVREQGETNODES_H_

#include <SimpleJSON.h>
#include <ReqServer.h>
#include "GCGVCallgraph.h"

/****************************************************************************
 *                Return a list of locations for function name
 ****************************************************************************/
/*
 * Request the list of function locations
 *
 * REQUEST:
 *    {
 *        name:  "TheFunctionToFind()"
 *        // -1 for full depth
 *        depth: 1,
 *        // cost or avCost
 *        sortBy: "cost"
 *    }
 *
 * REPLY:
 *    {
 *        nodes: [
 *            {
 *                path:   "Location/One",
 *                calls:  123,
 *                cost:   456,
 *                avCost: 3
 *            },
 *            {
 *                path:   "Location/Two",
 *                calls:  500,
 *                cost:   3000,
 *                avCost: 6
 *            }
 *        ]
 *    }
 *    In the event of their being no occurrences of the function, an empty path
 *    array will be returned.
 *
 */
class GCGV_ReqGetNodes: public RequestReplyHandler {
public:
    GCGV_ReqGetNodes(GCGV_Callgraph* ptr)
        : parent(ptr) { }

    virtual ~GCGV_ReqGetNodes();

    virtual std::string OnRequest(const char* JSON);

private:
    NewIntField(depth);
    NewStringField(name);
    NewStringField(sortBy);

    typedef SimpleParsedJSON<depth,name,sortBy> Request;

    enum SORT_BY {COST, AV_COST};
    SORT_BY sort = COST;

    class SortedNodes: public std::multimap<long,NodePtr> {
    public:
        void AddTree(
            NodePtr root,
            SORT_BY sort,
            const std::string& filter,
            int depth);
        void AddNode(NodePtr node, SORT_BY sort);
    };
    SortedNodes sortedNodes;

    GCGV_Callgraph*    parent;
    SimpleJSONBuilder  reply;

    Request request;

    void SetupRequest(const char* JSON);
    void FindNodes();
    void BuildReply();
};

#endif /* GCGVREQGETNODES_H_ */
