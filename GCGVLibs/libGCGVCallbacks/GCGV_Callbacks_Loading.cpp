/*
 * GCGV_Callbacks_Loading.cpp
 *
 *  Created on: 8 Feb 2015
 *      Author: lhumphreys
 */


#include "GCGV_Callbacks_Loading.h"
#include "include/wrapper/cef_helpers.h"

GCGV_Callbacks_Loading::GCGV_Callbacks_Loading() {

}

GCGV_Callbacks_Loading::~GCGV_Callbacks_Loading() {
}

void GCGV_Callbacks_Loading::OnLoadError(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl)
{
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
			"<h2>Failed to load URL " << std::string(failedUrl) <<
			" with error " << std::string(errorText) << " (" << errorCode <<
			").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}
