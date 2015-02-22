/*
 * GCGVBrowser_Callbacks_Display.h
 *
 *  Created on: 8 Feb 2015
 *      Author: lhumphreys
 */

#ifndef GCGVBROWSER_CALLBACKS_DISPLAY_H_
#define GCGVBROWSER_CALLBACKS_DISPLAY_H_

#include <include/cef_display_handler.h>

/**
 * Handles call-backs releated to the display of browsers.
 *
 * This gives us control over what is actually displayed to the user (e.g we can
 * block console messages), but also allows us to handle events which require
 * interaction with GTK. (e.g title display)
 */
class GCGVBrowser_Callbacks_Display: public CefDisplayHandler {
public:
	/**
	 * Default C'tor - nothing to do
	 */
	GCGVBrowser_Callbacks_Display();

	/**
	 * D'tor - nothing to do (but see reference counting...)
	 */
	virtual ~GCGVBrowser_Callbacks_Display();

	/**
	 * The title has changed - must update the GTK window
	 */
	virtual void OnTitleChange(
			CefRefPtr<CefBrowser> browser,
			const CefString& title) OVERRIDE;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(GCGVBrowser_Callbacks_Display);

};

#endif /* GCGVBROWSER_CALLBACKS_DISPLAY_H_ */
