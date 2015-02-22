/*
 * GCGVBrowserRequsts.cpp
 *
 *  Created on: 22 Feb 2015
 *      Author: lhumphreys
 */

#include "GCGVBrowserRequsts.h"

#include "GCGV_Callbacks.h"

GCGVBrowser_Requests::GCGVBrowser_Requests() {
    // TODO Auto-generated constructor stub

}

GCGVBrowser_Requests::~GCGVBrowser_Requests() {
}

bool GCGVBrowser_Requests::OnBeforeBrowse(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_redirect)
{
    bool cancel_navigation = false;

    GCGV_Callbacks::GetInstance()->GetJSHandler()->OnBeforeBrowse(
        browser,
        frame);

    return cancel_navigation;

}

void GCGVBrowser_Requests::OnRenderProcessTerminated(
    CefRefPtr<CefBrowser> browser,
    TerminationStatus status)
{
    GCGV_Callbacks::GetInstance()->GetJSHandler()->OnRenderProcessTerminated(
        browser);

}
