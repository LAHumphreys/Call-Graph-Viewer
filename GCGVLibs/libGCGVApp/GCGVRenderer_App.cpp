/*
 * GCGVRenderer_App.cpp
 *
 *  Created on: 7 Feb 2015
 *      Author: lhumphreys
 */

#include "GCGVRenderer_App.h"

GCGVRenderer_App::GCGVRenderer_App()
    : jsHandler_(new GCGV_JSHandler)
{
}

void GCGVRenderer_App::OnContextCreated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context)
{
    jsHandler_->OnContextCreated(
        browser,
        frame,
        context);
}

void GCGVRenderer_App::OnContextReleased(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context)
{
    jsHandler_->OnContextReleased(
        browser,
        frame,
        context);
}

GCGVRenderer_App::~GCGVRenderer_App() {
}

