/*
 * GCGVRenderer_App.h
 *
 *  Created on: 7 Feb 2015
 *      Author: lhumphreys
 */

#ifndef GCGVRENDERER_APP_H_
#define GCGVRENDERER_APP_H_

#include <include/cef_render_process_handler.h>
#include <GCGVJSHandler.h>

/**
 * Handle process level events on the Render processes (remembering there may be
 * one per site...).
 *
 * Responsibilities include handling messages from the browser process and
 * dealing with the java-script (V8) integration.
 */
class GCGVRenderer_App: public CefRenderProcessHandler {
public:

	/**
	 * C'tor - nothing to do (but see ref-counting...)
	 */
	GCGVRenderer_App();

	/**
	 * This callback is triggered immediate after the V8 context for a frame has
	 * been created.
	 *
	 * It is used to initialise the java-script environment.
	 */
	virtual void OnContextCreated(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) OVERRIDE;

  ///
  // Called immediately before the V8 context for a frame is released. No
  // references to the context should be kept after this method is called.
  ///
  virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context);


	/**
	 * D'tor - nothing to do (but see ref-counting...)
	 */
	virtual ~GCGVRenderer_App();
private:
	CefRefPtr<GCGV_JSHandler>   jsHandler_;

	// Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(GCGVRenderer_App);
};

#endif /* GCGVRENDERER_APP_H_ */
