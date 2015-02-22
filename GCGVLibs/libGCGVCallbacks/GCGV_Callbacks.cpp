// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "GCGV_Callbacks.h"

#include <sstream>
#include <string>

namespace {

GCGV_Callbacks* g_instance = NULL;

}  // namespace

GCGV_Callbacks::GCGV_Callbacks()
    : displayHandler_(new GCGVBrowser_Callbacks_Display),
      lifeSpanHandler_(new GCGVBrowser_Callbacks_LifeSpan),
      loadHandler_(new GCGV_Callbacks_Loading)
{
	DCHECK(!g_instance);
	g_instance = this;
}

GCGV_Callbacks::~GCGV_Callbacks() {
	g_instance = NULL;
}

// static
GCGV_Callbacks* GCGV_Callbacks::GetInstance() {
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
