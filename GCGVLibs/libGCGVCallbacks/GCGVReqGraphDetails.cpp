/*
 * GCGVReqGraphDetails.cpp
 *
 *  Created on: 6 Apr 2015
 *      Author: lhumphreys
 */

#include "GCGVReqGraphDetails.h"
#include <nodeConfig.h>

std::string GCGVCallgraph_GetGraphDetails::OnRequest(const char* JSON) {
    reply.Clear();

    reply.Get<fileName>() = parent->FileName();

    parent->PWD().GetPath(reply.Get<path>());

    LookupUnits();

    return reply.GetJSONString();
}

void GCGVCallgraph_GetGraphDetails::LookupUnits() {
    for ( const size_t& i : NodeConfig::Instance().DisplayIdxs() ) {
        const std::string& unitName =
                NodeConfig::Instance().CostFactory().GetName(i);
        reply.Get<units>().push_back(unitName);
    }
}
