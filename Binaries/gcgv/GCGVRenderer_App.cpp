/*
 * GCGVRenderer_App.cpp
 *
 *  Created on: 7 Feb 2015
 *      Author: lhumphreys
 */

#include "GCGVRenderer_App.h"

GCGVRenderer_App::GCGVRenderer_App() {

}

void GCGVRenderer_App::OnContextCreated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context)
{
	// Retrieve the global object...
	CefRefPtr<CefV8Value> object = context->GetGlobal();

	CefRefPtr<CefV8Value> str = CefV8Value::CreateString("Hello from CEF!");

	object->SetValue("helloStr",str,V8_PROPERTY_ATTRIBUTE_NONE);
}

GCGVRenderer_App::~GCGVRenderer_App() {
}

