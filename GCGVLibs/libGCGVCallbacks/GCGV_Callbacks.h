// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_

#include "include/cef_client.h"
#include "GCGVBrowser_Callbacks_Display.h"
#include "GCGVBrowser_Callbacks_LifeSpan.h"
#include "GCGV_Callbacks_Loading.h"
#include "GCGVBrowserRequsts.h"
#include <GCGVJSHandler.h>

/**
 * Handle events that occur in the application. Note that this could be in
 * either the browser or the renderer processes.
 *
 * This class derives from the CefClient class, which provides accessors to
 * classes which handle call-backs into the browser process. While the
 * GCGVBrowser_App and GCGVRenderer_App classes handle the most significant
 * call-backs (e.g creation), the vast majority of the "run of the mill"
 * call-backs are handled by the objects returned by CefClient.
 */
class GCGV_Callbacks: public CefClient {
public:
    /**
     * C'tor - Initialise the child handlers
     */
    GCGV_Callbacks();

    /**
     * D'tor - nothing to do (ref-counting will take care of the dynamic objects)
     */
    virtual ~GCGV_Callbacks();

    // Provide access to the single global instance of this object.
    static GCGV_Callbacks* GetInstance();

    /******************************************************
     *              CEF Interface
     ****************************************************/

    /**
     * Return the object responsible for handling call-backs resulting from changes
     * that will be displayed to the user.
     *
     * Examples include the page title and console logging.
     */
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE;

    /**
     * Return the object responsible for handling notifications related to the
     * creation / destruction of browsers.
     */
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE;

    /**
     * Return the object responsible for handling load events (including failures)
     * on the page.
     *
     * Note these events could be triggered from either the browser or the
     * renderer...
     */
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;

    /**
     * Return the object responsible for handling browser requests
     */
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE;

    /*
     * Handle a message from another process.
     *
     * Note that we do not know what process we are on when the function is called..
     */
    virtual bool OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message);

        /******************************************************
         *              Custom Members
         ****************************************************/

        /**
         * Return the Java Script handler
         */
        CefRefPtr<GCGV_JSHandler> GetJSHandler();

    private:

        CefRefPtr<GCGVBrowser_Callbacks_Display> displayHandler_;
        CefRefPtr<GCGVBrowser_Callbacks_LifeSpan> lifeSpanHandler_;
        CefRefPtr<GCGV_Callbacks_Loading> loadHandler_;
        CefRefPtr<GCGVBrowser_Requests> requestHandler_;
        CefRefPtr<GCGV_JSHandler> jsHandler_;

        // Include the default reference counting implementation.
        IMPLEMENT_REFCOUNTING(GCGV_Callbacks)
        ;
    }
    ;

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
