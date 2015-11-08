// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <GCGVCallgraph.h>
#include "ReqServer.h"

// Entry point function for all processes.
int main(int argc, char* argv[]) {

    RequestServer server;

    GCGV_Callgraph graph;
    graph.InstallHandlers(server);

    short port = 9002;
    if (argc > 1 ) {
        std::string sport(argv[1]);
        std::stringstream strport(sport);
        strport >> port;
    }
    server.HandleRequests(port);

    return 0;
}
