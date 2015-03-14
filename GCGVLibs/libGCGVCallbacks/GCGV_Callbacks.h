// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_

#include "include/cef_client.h"
#include <CefBaseClient.h>
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

    static void InstallNewHandlers(CefBaseClient& client);

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

        CefRefPtr<GCGV_JSHandler> jsHandler_;

        // Include the default reference counting implementation.
        IMPLEMENT_REFCOUNTING(GCGV_Callbacks)
    };

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_HANDLER_H_
