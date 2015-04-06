/*
 * CefBaseReqNavigate.h
 *
 *  Created on: 6 Apr 2015
 *      Author: lhumphreys
 */

#ifndef CEFBASEREQNAVIGATE_H_
#define CEFBASEREQNAVIGATE_H_

#include <CefBaseRequestReplies.h>
#include <SimpleJSON.h>
#include <GCGVBrowser_App.h>

/*
 * Request navigation to a new url. (This simulates a user providing doing a new
 * navigation, as opposed to a re-direct).
 *
 * REQUEST:
 *    {
 *         file: "Analyse.html"
 *    }
 *
 * REPLY:
 *    NONE
 *
 *
 * WARNING: The use of this query introduces security concerns - we are
 *          by-passing the cross origin defences.
 *
 *          We are only allowing access to files which mitigates some of these
 *          concerns.
 */
class GCGVReqNavigate: public CefBaseJSRequestReply {
public:
    GCGVReqNavigate(GCGVBrowser_App* app): parent(app) { }
    virtual ~GCGVReqNavigate();

    virtual std::string OnRequest(RequestContext& context);
private:
    NewStringField(file);

    typedef SimpleParsedJSON<file> Request;

    Request request;
    GCGVBrowser_App* parent;
};

#endif /* CEFBASEREQNAVIGATE_H_ */
