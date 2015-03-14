// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "GCGV_Callbacks.h"

#include <sstream>
#include <string>

#include "CefBaseDisplayGTKDefaultHandler.h"
#include "GCGVBrowser_Callbacks_LifeSpan.h"
#include <CefBaseLoadDefaultHandler.h>
#include "GCGVBrowserRequsts.h"
#include <CefBaseLifeSpanDefaultHandler.h>

#include <logger.h>

namespace {

    GCGV_Callbacks* g_instance = NULL;

}  // namespace

void GCGV_Callbacks::InstallNewHandlers(CefBaseClient& client) {
    client.InstallMessagerHandler(
        std::shared_ptr<CefClient>(new GCGV_Callbacks));

    client.DisplayHandler().InstallHandler(
        std::shared_ptr<CefDisplayHandler>(
            new CefBaseDisplayDefaultGTKHandler));

    client.LifeSpanHandler().InstallHandler(
        std::shared_ptr<CefLifeSpanHandler>(
            new CefBaseLifeSpanDefaultHandler));

    client.LifeSpanHandler().InstallHandler(
        std::shared_ptr<CefLifeSpanHandler>(
            new GCGVBrowser_Callbacks_LifeSpan));

    client.LoadHandler().InstallHandler(
        std::shared_ptr<CefLoadHandler>(
            new CefBaseLoadDefaultHandler()));

    client.RequestHandler().InstallHandler(
        std::shared_ptr<CefRequestHandler>(
            new GCGVBrowser_Requests));
}


GCGV_Callbacks::GCGV_Callbacks()
        : jsHandler_(new GCGV_JSHandler)
{
    DCHECK(!g_instance);
    g_instance = this;
}

GCGV_Callbacks::~GCGV_Callbacks() {
    g_instance = NULL;
}

// static
GCGV_Callbacks* GCGV_Callbacks::GetInstance() {
    if (!g_instance) {
        LOG_FROM(
            LOG_ERROR,
            "GCGV_Callbacks::GetInstance",
            "FATAL: There is no instance to return!");
        abort();
    }
    return g_instance;
}

CefRefPtr<GCGV_JSHandler> GCGV_Callbacks::GetJSHandler() {
    return jsHandler_;
}

bool GCGV_Callbacks::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message)
{
    bool handled = false;

    handled = GetJSHandler()->OnProcessMessageReceived(
        browser,
        source_process,
        message);

    return handled;
}

