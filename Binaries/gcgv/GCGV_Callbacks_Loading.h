/*
 * GCGV_Callbacks_Loading.h
 *
 *  Created on: 8 Feb 2015
 *      Author: lhumphreys
 */

#ifndef GCGV_CALLBACKS_LOADING_H_
#define GCGV_CALLBACKS_LOADING_H_

#include <include/cef_load_handler.h>

/**
 * Handle events related to page loading ( although note that this could be
 * individual gets / files within the page).
 *
 * The class is not process namespaced since the documentation only states that
 * the methods could be called in "either the browser or the renderer process".
 */
class GCGV_Callbacks_Loading: public CefLoadHandler {
public:
	/**
	 * C'tor, nothing to do
	 */
	GCGV_Callbacks_Loading();

	/**
	 * D'tor, nothing to do (but see ref counting...)
	 */
	virtual ~GCGV_Callbacks_Loading();

	/**
	 * A load has failed on the on page.
	 *
	 * If the load was the main page ( not some internal get) put some useful error
	 * text in the frame.
	 */
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			ErrorCode errorCode,
			const CefString& errorText,
			const CefString& failedUrl) OVERRIDE;
private:

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(GCGV_Callbacks_Loading);
};

#endif /* GCGV_CALLBACKS_LOADING_H_ */
