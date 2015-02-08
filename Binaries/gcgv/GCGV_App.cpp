// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "GCGV_App.h"

#include <string>

#include "GCGVBrowser_Callbacks.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"


GCGV_App::GCGV_App()
    : browserApp_(new GCGVBrowser_App),
      rendererApp_(new GCGVRenderer_App)
{
}

CefRefPtr<CefBrowserProcessHandler> GCGV_App::GetBrowserProcessHandler() {
	return browserApp_;
}

GCGV_App::~GCGV_App() {
}

CefRefPtr<CefRenderProcessHandler> GCGV_App::GetRenderProcessHandler() {
	return rendererApp_;
}
