// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "GCGV_Callbacks.h"

#include <sstream>
#include <string>

#include "CefBaseDisplayGTKDefaultHandler.h"
#include <CefBaseLoadDefaultHandler.h>
#include <CefBaseLifeSpanDefaultHandler.h>

#include <logger.h>

void GCGV_Callbacks::InstallNewHandlers(CefBaseClient& client) {
    std::shared_ptr<GCGV_Callbacks> gcgv_client(new GCGV_Callbacks);
    client.InstallMessagerHandler(gcgv_client);

    client.DisplayHandler().InstallHandler(
        std::shared_ptr<CefDisplayHandler>(
            new CefBaseDisplayDefaultGTKHandler));

    client.LifeSpanHandler().InstallHandler(
        std::shared_ptr<CefLifeSpanHandler>(
            new CefBaseLifeSpanDefaultHandler));

    client.LoadHandler().InstallHandler(
        std::shared_ptr<CefLoadHandler>(
            new CefBaseLoadDefaultHandler()));

    gcgv_client->GetJSHandler()->InstallHandler(client.Parent());
}


GCGV_Callbacks::GCGV_Callbacks()
    : jsHandler_(new CefBaseJSHandler("gcgvQuery","gcgvQueryCancel"))
{
}

GCGV_Callbacks::~GCGV_Callbacks() {
}

CefRefPtr<CefBaseJSHandler> GCGV_Callbacks::GetJSHandler() {
    return jsHandler_;
}
