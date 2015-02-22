/*
 * GCGVMessageRouter.h
 *
 *  Created on: 22 Feb 2015
 *      Author: lhumphreys
 */

#ifndef GCGVMESSAGEROUTER_H_
#define GCGVMESSAGEROUTER_H_

#include <include/wrapper/cef_message_router.h>
#include <string>

class GCGV_MessageRouter {
public:
    /*
     * Create a new message router
     */
    GCGV_MessageRouter(std::string queryFn, std::string cancelFn);

    virtual ~GCGV_MessageRouter();

    CefRefPtr<CefMessageRouterBrowserSide> GetBrowserSideRouter();

    CefRefPtr<CefMessageRouterRendererSide> GetRendererSideRouter();


private:
    CefRefPtr<CefMessageRouterBrowserSide>  browserSideRouter_;
    CefRefPtr<CefMessageRouterRendererSide> rendererSideRouter_;

	IMPLEMENT_REFCOUNTING(GCGV_MessageRouter);
};

#endif /* GCGVMESSAGEROUTER_H_ */
