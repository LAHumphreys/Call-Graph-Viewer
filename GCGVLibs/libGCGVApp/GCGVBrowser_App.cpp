/*
 * GCGVBrowserApp.cpp
 *
 *  Created on: 7 Feb 2015
 *      Author: lhumphreys
 */

#include "GCGVBrowser_App.h"
#include "GCGV_Callbacks.h"

#include "include/wrapper/cef_helpers.h"

#include <env.h>

GCGVBrowser_App::GCGVBrowser_App(CefBaseApp& _app)
    : app(_app)
{
}

void GCGVBrowser_App::OnContextInitialized() {
	// This should be called from the browser process..
	CEF_REQUIRE_UI_THREAD();

	// Information used when creating the native window.
	CefWindowInfo window_info;

	// Specify CEF browser settings here.
	CefBrowserSettings browser_settings;

	std::string url = GetStartUrl();

	// Create the first browser window.
    CefBrowserHost::CreateBrowser(
        window_info,
        app.GetClient(),
        url,
        browser_settings,
        NULL);
}

GCGVBrowser_App::~GCGVBrowser_App() {
}

std::string GCGVBrowser_App::GetStartUrl() {
	std::string url;

	// Check if a "--url=" value was provided via the command-line. If so, use
	// that instead of the default URL.
	CefRefPtr<CefCommandLine> command_line =
			CefCommandLine::GetGlobalCommandLine();

	url = command_line->GetSwitchValue("url");

	if (url.empty()) {
		std::string pwd = ENV::GetEnvString("PWD");
		url = "file://" + pwd + "/Binaries/gcgv/Content/StartPage.html";
	}
	return url;
}
