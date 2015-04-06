/*
 * GCGVCallgraph.h
 *
 *  Created on: 6 Apr 2015
 *      Author: lhumphreys
 */

#ifndef GCGVCALLGRAPH_H_
#define GCGVCALLGRAPH_H_

#include "callgrindTree.h"
#include <memory>
#include <node.h>

class CefBaseJSRequestReplyHandler;

class GCGV_Callgraph {
public:
    GCGV_Callgraph();

    void InstallHandlers(CefBaseJSRequestReplyHandler& ReqReps);

    virtual ~GCGV_Callgraph();

    bool LoadGraph(const std::string& fname);

    const std::string& FileName() const { return fname; }

    NodePtr CWD() { return cwd; }

private:
    std::unique_ptr<CallgrindNative> graph;
    NodePtr                          cwd;
    std::string                      fname;
};

#endif /* GCGVCALLGRAPH_H_ */
