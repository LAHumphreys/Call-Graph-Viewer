/*
 * GCGVMessageRouter.cpp
 *
 *  Created on: 22 Feb 2015
 *      Author: lhumphreys
 */

#include "GCGVMessageRouter.h"

GCGV_MessageRouter::GCGV_MessageRouter(
    std::string queryFn,
    std::string cancelFn)

{
    CefMessageRouterConfig config;
    config.js_query_function = queryFn;
    config.js_cancel_function = cancelFn;

    browserSideRouter_ = CefMessageRouterBrowserSide::Create(config);
    rendererSideRouter_ = CefMessageRouterRendererSide::Create(config);

}

GCGV_MessageRouter::~GCGV_MessageRouter() {
}

CefRefPtr<CefMessageRouterBrowserSide> GCGV_MessageRouter::GetBrowserSideRouter()
{
    return browserSideRouter_;
}

CefRefPtr<CefMessageRouterRendererSide> GCGV_MessageRouter::GetRendererSideRouter()
{
    return rendererSideRouter_;
}
