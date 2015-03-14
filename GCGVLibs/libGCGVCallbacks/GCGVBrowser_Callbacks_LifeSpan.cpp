/*
 * GCGVBrowser_Callbacks_LifeSpan.cpp
 *
 *  Created on: 8 Feb 2015
 *      Author: lhumphreys
 */

#include "GCGVBrowser_Callbacks_LifeSpan.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"

#include "GCGV_Callbacks.h"

GCGVBrowser_Callbacks_LifeSpan::GCGVBrowser_Callbacks_LifeSpan()
{

}

void GCGVBrowser_Callbacks_LifeSpan::OnBeforeClose(
		CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();
	GCGV_Callbacks::GetInstance()->GetJSHandler()->OnBeforeClose(browser);
}
