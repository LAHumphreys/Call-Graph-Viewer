/*
 * CefBaseReqNavigate.cpp
 *
 *  Created on: 6 Apr 2015
 *      Author: lhumphreys
 */

#include "GCGVReqNavigate.h"
#include <logger.h>

GCGVReqNavigate::~GCGVReqNavigate() {
    // TODO Auto-generated destructor stub
}

std::string GCGVReqNavigate::OnRequest(RequestContext& context) {
    request.Clear();
    static std::string error;

    if ( !request.Parse(context.request.c_str(), error))
    {
        throw CefBaseInvalidRequestException{0,error};
    }

    std::string url;

    url = parent->GetBaseUrl() + "/" + request.Get<file>();

    SLOG_FROM(
        LOG_VERBOSE,
        "GCGVReqNavigate::OnRequest",
        "Initiating url request for: " << url
        )

    context.browser->GetMainFrame()->LoadURL(url);


    // The javascript context will now be dead...
    throw CefBaseAbdandonRequest{};
}
