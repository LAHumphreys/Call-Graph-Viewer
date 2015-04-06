#include "GCGV_Callbacks.h"

#include <sstream>
#include <string>

#include "CefBaseDisplayGTKDefaultHandler.h"
#include <CefBaseLoadDefaultHandler.h>
#include <CefBaseLifeSpanDefaultHandler.h>
#include <CefBaseReqFileList.h>
#include <include/cef_origin_whitelist.h>

#include <logger.h>

std::shared_ptr<GCGV_Callbacks> GCGV_Callbacks::InstallNewHandlers(CefBaseClient& client, bool testLibs) {
    std::shared_ptr<GCGV_Callbacks> gcgv_client(new GCGV_Callbacks);
    client.InstallMessagerHandler(gcgv_client);

    if ( !testLibs)
    {
        client.DisplayHandler().InstallHandler(
            std::shared_ptr<CefDisplayHandler>(
                new CefBaseDisplayDefaultGTKHandler));

        client.LifeSpanHandler().InstallHandler(
            std::shared_ptr<CefLifeSpanHandler>(
                new CefBaseLifeSpanDefaultHandler));
    }

    client.LoadHandler().InstallHandler(
        std::shared_ptr<CefLoadHandler>(
            new CefBaseLoadDefaultHandler()));

    gcgv_client->GetJSHandler()->InstallHandler(client.Parent());

    InstallWhiteList();

    return gcgv_client;
}


GCGV_Callbacks::GCGV_Callbacks()
    : jsHandler_(new CefBaseJSHandler("gcgvQuery","gcgvQueryCancel"))
{
    jsHandler_->ReqReps().Install<CefBaseReqFileList>("GCGVFileList");

    callGraph_.InstallHandlers(GetJSHandler()->ReqReps());
}

GCGV_Callbacks::~GCGV_Callbacks() {
}

void GCGV_Callbacks::InstallWhiteList() {
}

CefRefPtr<CefBaseJSHandler> GCGV_Callbacks::GetJSHandler() {
    return jsHandler_;
}
