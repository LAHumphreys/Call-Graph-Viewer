// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "GCGV_Callbacks.h"

#include <sstream>
#include <string>

#include <logger.h>

namespace {

GCGV_Callbacks* g_instance = NULL;

}  // namespace

GCGV_Callbacks::GCGV_Callbacks()
    : displayHandler_(new GCGVBrowser_Callbacks_Display),
      lifeSpanHandler_(new GCGVBrowser_Callbacks_LifeSpan),
      loadHandler_(new GCGV_Callbacks_Loading),
      requestHandler_(new GCGVBrowser_Requests),
      jsHandler_(new GCGV_JSHandler)
{
	DCHECK(!g_instance);
	g_instance = this;
}

GCGV_Callbacks::~GCGV_Callbacks() {
	g_instance = NULL;
}

// static
GCGV_Callbacks* GCGV_Callbacks::GetInstance() {
    if ( !g_instance)
    {
        LOG_FROM(
            LOG_ERROR,
            "GCGV_Callbacks::GetInstance",
            "FATAL: There is no instance to return!");
        abort();
    }
	return g_instance;
}

CefRefPtr<CefDisplayHandler> GCGV_Callbacks::GetDisplayHandler() {
	return displayHandler_;
}

CefRefPtr<CefLifeSpanHandler> GCGV_Callbacks::GetLifeSpanHandler() {
	return lifeSpanHandler_;
}

CefRefPtr<CefLoadHandler> GCGV_Callbacks::GetLoadHandler() {
	return loadHandler_;
}

CefRefPtr<CefRequestHandler> GCGV_Callbacks::GetRequestHandler() {
    return requestHandler_;
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
