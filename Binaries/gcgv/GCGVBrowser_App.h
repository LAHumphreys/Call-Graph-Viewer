/*
 * GCGVBrowserApp.h
 *
 *  Created on: 7 Feb 2015
 *      Author: lhumphreys
 */

#ifndef GCGVBROWSERAPP_H_
#define GCGVBROWSERAPP_H_

#include <include/cef_browser_process_handler.h>

/**
 * Handles application level events in the UI thread (Browser). Its primary
 * responsibility is to handle the initialisation of the CEF context
 * (OnContextInitialized) in the browser process.
 *
 * All method are executed on the UI THREAD (Browser process)
 */
class GCGVBrowser_App: public CefBrowserProcessHandler {
public:
	/**
	 * Default C'tor - nothing to do.
	 */
	GCGVBrowser_App();

	/**
	 * The CEF context has been created. Create a new window and display it.
	 *
	 * This method is responsible for setting the initial paramaters of the window
	 * (including its url).
	 *
	 * Finally the method creates a new instance of GCGV_Callbacks, which is
	 * used by this window for event handling.
	 *
	 */
	virtual void OnContextInitialized() OVERRIDE;

	/**
	 * D'tor - nothing to do (but see reference counting...)
	 */
	virtual ~GCGVBrowser_App();
private:
	/**
	 * Calculate the starting URL. This will default to the Content provided by the
	 * app, but can be overridden with the --url flag ( this is useful for, amongst
	 * other things, Brackets integration)
	 *
	 * @returns The url that the browser should initialise to
	 */
	std::string GetStartUrl();

	// Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(GCGVBrowser_App)
};

#endif /* GCGVBROWSERAPP_H_ */
