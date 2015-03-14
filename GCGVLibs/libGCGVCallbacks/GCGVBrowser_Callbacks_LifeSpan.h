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
	 * A browser is about to be destroyed - stop tracking it.
	 *
	 * In addition, we should be cleaning up any references we still have to it.
	 */
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(GCGVBrowser_Callbacks_LifeSpan);
};

#endif /* GCGVBROWSER_CALLBACKS_LIFESPAN_H_ */
