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

    bool LoadGraph(const std::string& fname, NodePtr root);

    const std::string& FileName() const { return fname; }

    NodePtr CWD() { return cwd; }

    /**
     * Return the string path of the current node.
     */
    Path PWD() const;

    /**
     * Get the path (from ROOT) of the node
     */
    static Path GetPath(NodePtr node);

    /**
     * Change the current working node to the path, relative to the current
     * node.
     *
     * If the path is not valid, the function will return false, and the CWD
     * will be unchanged.
     *
     * @param path   Node to change to.
     *
     * @returns TRUE if the node was changed, false otherwise.
     *
     */
    bool ChangeNode(const Path& path);

private:
    class GraphStorage {
    public:
        GraphStorage ();

        bool LoadGraph(const std::string& fname);

        bool LoadStack(const std::string& fname);

        NodePtr RootNode();
    private:
        NodePtr                          root;
        std::unique_ptr<CallgrindNative> graph;
    };
    GraphStorage                     store;
    NodePtr                          cwd;
    NodePtr                          root;
    std::string                      fname;
};

#endif /* GCGVCALLGRAPH_H_ */
