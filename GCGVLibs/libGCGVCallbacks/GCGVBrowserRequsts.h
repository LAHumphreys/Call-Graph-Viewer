/*
 * GCGVBrowserRequsts.h
 *
 *  Created on: 22 Feb 2015
 *      Author: lhumphreys
 */

#ifndef GCGVBROWSERREQUSTS_H_
#define GCGVBROWSERREQUSTS_H_

#include <include/cef_request_handler.h>

class GCGVBrowser_Requests: public CefRequestHandler {
public:
    GCGVBrowser_Requests();
    virtual ~GCGVBrowser_Requests();
    /****************************************************************************
     *                        CEF Interface
     ****************************************************************************/

    ///
    // Called on the UI thread before browser navigation. Return true to cancel
    // the navigation or false to allow the navigation to proceed. The |request|
    // object cannot be modified in this callback.
    // CefLoadHandler::OnLoadingStateChange will be called twice in all cases.
    // If the navigation is allowed CefLoadHandler::OnLoadStart and
    // CefLoadHandler::OnLoadEnd will be called. If the navigation is canceled
    // CefLoadHandler::OnLoadError will be called with an |errorCode| value of
    // ERR_ABORTED.
    ///
    /*--cef()--*/
    virtual bool OnBeforeBrowse(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool is_redirect);

    ///
    // Called on the browser process UI thread when the render process
    // terminates unexpectedly. |status| indicates how the process
    // terminated.
    ///
    /*--cef()--*/
    virtual void OnRenderProcessTerminated(
        CefRefPtr<CefBrowser> browser,
        TerminationStatus status);

private:

    IMPLEMENT_REFCOUNTING(GCGVBrowser_Requests);
};

#endif /* GCGVBROWSERREQUSTS_H_ */
