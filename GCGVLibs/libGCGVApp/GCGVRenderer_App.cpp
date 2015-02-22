/*
 * GCGVRenderer_App.cpp
 *
 *  Created on: 7 Feb 2015
 *      Author: lhumphreys
 */

#include "GCGVRenderer_App.h"
#include <CEFJSUtils.h>
#include <CEFJSLogging.h>

GCGVRenderer_App::GCGVRenderer_App() {

}

void GCGVRenderer_App::OnContextCreated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context)
{
	CEFJSUtils::SetGlobalVariable("helloStr","Hello from CEF!",context);
	CEFJSLogging::AddFormater(context);
}

GCGVRenderer_App::~GCGVRenderer_App() {
}

