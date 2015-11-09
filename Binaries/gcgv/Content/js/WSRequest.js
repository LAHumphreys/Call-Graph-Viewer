/*global $, console */

/**
 * Web Socket Request-Reply module.
 * --------------------------------
 *
 *  (c) Luke Humphreys 2015
 *
 *  This module provides a simple wrapper around 
 */
function wsrequest_NewRequestManager(url) {
    "use strict";
    return function () {
        "use strict";
        var reqNum = 0, activeRequests = [];

        function deleteRequest(id) {
            "use strict";
            console.log("Cleaning up request with id: " + id);
            activeRequests.splice(id,1);
        }

        function getNextId() { 
            "use strict";
            reqNum += 1;
            return reqNum;
        }

        return {
            newRequest: function (reqName, msg, context) { 
                "use strict";
                var reqid = getNextId();
                context._onDone = function () { deleteRequest(reqid); }
                context._server = url;
                activeRequests[reqid] = new WSRequest(reqName, msg, context);
                return reqid;
            }
        };
    }();
}

function WSRequest(reqName, msg, context) {
    "use strict";
    this._request = reqName + " " + JSON.stringify(msg);
    if (context) {
        extend(this, context);
    }
    this.startRequest();
}

WSRequest.prototype = {
    onResponse: null,
    onError: null,
    // Used by WSRequest_NewRequestManager
    _onDone: null,
    _server: "",
    // Internal data
    _socket: null,
    _request: null,
    _closing: false,

    startRequest: function () {
        "use strict";
        this._socket = new WebSocket(this._server);
        this._socket.onopen = $.proxy(this._onOpen, this);
        this._socket.onmessage = $.proxy(this._onMessage, this);
        this._socket.onclose = $.proxy(this._onClose, this);
        this._socket.onerror = $.proxy(this._onError, this);
    },

    close: function() {
        "use strict";
        this._socket.close();
        this._socket = null;
        this._closing = true;
    },

    _onOpen: function (evt) {
        "use strict";
        console.log("Sending request: " + this._request);
        this._socket.send(this._request);
    },

    _onClose: function (evt) {
        "use strict";

        if(this.closing === false) {
            console.log("Server connection lost!");
            this._reportError(0,"Server connection lost!");
        }

        if(this._onDone) { 
            this._onDone();
        }
    },

    _onError: function (evt) {
        "use strict";
        console.log("An internal error occurred: " + evt.data);
        this._reportError(-1,evt.data);

        this.close();
    },

    _onMessage: function(evt) {
        "use strict";
        var msg = evt.data, err = "";

        console.log("Got Message: " + msg);

        if (msg.substring(0,6) === "ERROR ") {
            err = JSON.parse(msg.substring(6));
            this._reportError(err.errorNumber, err.errorText);
        } else {
            if ( this.onResponse ) {
                this.onResponse(JSON.parse(msg));
            }
        }

        // Modelling a one-shot request-reply message...
        this.close();
    },

    _reportError: function(code, err) {
        console.log("Something went wrong: " + err);
        if (this.onError) {
            this.onError(code,err);
        }
    }
};
