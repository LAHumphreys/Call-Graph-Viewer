/*
 * GCGVCallgraphGetGraph.h
 *
 *  Created on: 31 May 2015
 *      Author: lhumphreys
 */

#ifndef GCGVCALLGRAPHGETGRAPH_H_
#define GCGVCALLGRAPHGETGRAPH_H_

#include <SimpleJSON.h>
#include <ReqServer.h>
#include "GCGVCallgraph.h"

/**
 * Request the callgraph from the current node.
 *
 * REQUEST
 *   {
 *     depth: 2
 *   }
 * REPLY
 *   {
 *     name: "ROOT",
 *     shortName: "ROOT",
 *     children: [
 *        {
 *          name: "main",
 *          shortName: "main",
 *          children: [
 *            {
 *               name: "odds"
 *               shortName: "odds",
 *            },
 *            {
 *               name: "evens"
 *               shortName: "evens",
 *            },
 *          ]
 *        }
 *     ]
 *   }
 *
 */
class GCGVCallgraph_ReqGraph: public RequestReplyHandler {
public:
    GCGVCallgraph_ReqGraph(GCGV_Callgraph* ptr)
        : parent(ptr) { }

    virtual ~GCGVCallgraph_ReqGraph();

    virtual std::string OnRequest(const char* JSON);

    /**
     * Add the details of the current node to the result JSON.
     *
     *   node:  The node to add
     *   depth: The depth above the cut-off point. (A depth of 0 implies no
     *          children will be added)
     */
    void AddNode(NodePtr node, size_t depth);
private:
    NewUIntField(depth);

    typedef SimpleParsedJSON<depth> Request;

    Request request;
    GCGV_Callgraph*    parent;
    SimpleJSONBuilder  builder;
};

#endif /* GCGVCALLGRAPHGETGRAPH_H_ */
