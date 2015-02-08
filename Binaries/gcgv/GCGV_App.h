// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"
#include "GCGVBrowser_App.h"
#include "GCGVRenderer_App.h"

/**
 * The core GCGV application. This is used by all CEF processes to get
 * information pertaining specifically to themselves.
 *
 * The class derives from CefApp, and overrides the getters for handlers used by
 * the application.
 */
class GCGV_App : public CefApp
{
public:
	/**
	 * C'tor.
	 *
	 * No special behaviour.
	 */
	GCGV_App();

	virtual ~GCGV_App();

	// CefApp methods:
	/**
	 * Return an object which handles
	 *
	 * @returns A reference to the
	 */
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
			OVERRIDE;

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE;
private:
    CefRefPtr<GCGVBrowser_App>  browserApp_;
    CefRefPtr<GCGVRenderer_App> rendererApp_;

	// Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(GCGV_App);
};

#endif
