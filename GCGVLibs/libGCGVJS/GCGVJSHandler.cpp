/*
 * GCGVJSHandler.cpp
 *
 *  Created on: 22 Feb 2015
 *      Author: lhumphreys
 */

#include "GCGVJSHandler.h"

#include <CEFJSLogging.h>
#include "include/wrapper/cef_helpers.h"

GCGV_JSHandler::GCGV_JSHandler()
   : router_("gcgvQuery","gcgvQueryCancel")
{
}

GCGV_JSHandler::~GCGV_JSHandler() {
    // TODO Auto-generated destructor stub
}


void GCGV_JSHandler::RegisterJSFunctions(CefRefPtr<CefV8Context> context) {
	CEFJSLogging::AddFormater(context);
}

/****************************************************************************
 *                        Browser Events
 ****************************************************************************/

void GCGV_JSHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

    router_.GetBrowserSideRouter()->OnBeforeClose(browser);
}

void GCGV_JSHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

    router_.GetBrowserSideRouter()->OnRenderProcessTerminated(browser);
}

void GCGV_JSHandler::OnBeforeBrowse(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame)
{
	CEF_REQUIRE_UI_THREAD();

    router_.GetBrowserSideRouter()->OnBeforeBrowse(browser,frame);
}

bool GCGV_JSHandler::OnBrowserProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message)
{
	CEF_REQUIRE_UI_THREAD();

    bool handled = false;

    if (!handled)
    {
        handled = router_.GetBrowserSideRouter()->OnProcessMessageReceived(
            browser,
            source_process,
            message);
    }

    return handled;
}

/****************************************************************************
 *                        Renderer Events
 ****************************************************************************/

void GCGV_JSHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context)
{
    RegisterJSFunctions(context);

    router_.GetRendererSideRouter()->OnContextCreated(browser,frame,context);
}

void GCGV_JSHandler::OnContextReleased(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context)
{
    router_.GetRendererSideRouter()->OnContextReleased(
        browser,
        frame,
        context);
}

bool GCGV_JSHandler::OnRendererProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message)
{
    bool handled = false;

    if (!handled)
    {
        handled = router_.GetRendererSideRouter()->OnProcessMessageReceived(
            browser,
            source_process,
            message);
    }

    return handled;
}

bool GCGV_JSHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message)
{
    bool handled = false;
    if (CefCurrentlyOn(TID_RENDERER))
    {
        handled = OnRendererProcessMessageReceived(
            browser,
            source_process,
            message);
    }
    else if (CefCurrentlyOn(TID_UI))
    {
        handled = OnBrowserProcessMessageReceived(
            browser,
            source_process,
            message);
    }

    return handled;
}
