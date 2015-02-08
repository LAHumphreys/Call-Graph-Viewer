/*
 * GCGVBrowser_Callbacks_LifeSpan.h
 *
 *  Created on: 8 Feb 2015
 *      Author: lhumphreys
 */

#ifndef GCGVBROWSER_CALLBACKS_LIFESPAN_H_
#define GCGVBROWSER_CALLBACKS_LIFESPAN_H_

#include <include/cef_life_span_handler.h>

#include <list>

/**
 * Handle events related to the browser life-span.
 *
 * We use these call-backs to track the currently created browsers which have
 * not been destroyed.
 *
 * This allows us to request that they all shut-down, as well as being able to
 * handle the case where the last browser is terminated (in which case we can
 * shutdown).
 */
class GCGVBrowser_Callbacks_LifeSpan: public CefLifeSpanHandler {
public:
	/**
	 * C'tor - initialise members...
	 *
	 */
	GCGVBrowser_Callbacks_LifeSpan();

	/**
	 * D'tor - nothing to do (but see ref counting...)
	 */
	virtual ~GCGVBrowser_Callbacks_LifeSpan();

	/**
	 * A new browser has been created - add it to the list we are tracking
	 */
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;

	/**
	 * A browser is about to be destroyed - stop tracking it.
	 *
	 * In addition, we should be cleaning up any references we still have to it.
	 */
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	/**
	 *  Called by the framework to determine how the window should be closed. See
	 *  the parent documentation for a lengthy description.
	 *
	 *  In our method we allow the default close operation to occur simple use this
	 *  as a hook to determine if the main window is closing
	 */
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;


	/**
	 * Shutdown all open browsers
	 */
	void CloseAllBrowsers(bool force_close);

	/**
	 * The main browser window has received a close request, we are shutting down
	 */
	bool IsClosing() const { return is_closing_; }
private:
	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList browser_list_;

	bool is_closing_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(GCGVBrowser_Callbacks_LifeSpan);
};

#endif /* GCGVBROWSER_CALLBACKS_LIFESPAN_H_ */
