/*global $, document, alert, Spinner, alertify, installNavbar, console, Logging, Select */
var Application;

var ModelPrototype = {
    _serverConnection: null,
    /*
     * The document is ready, start the model
     */
    initialise: function () {
        "use strict";
        Application.modelReady();
    },

    initialiseRequestManager: function () {
        "use strict";
        this._serverConnection = wsrequest_NewRequestManager("ws://127.0.0.1:9003");
    },
    
    /*
     * Request data from the backend
     */
    startRequest: function (name, request, onSuccess, onFailure) {
        "use strict";
        this._serverConnection.newRequest(name,request, {
            onResponse: onSuccess,
            onError: onFailure
        });
        
        if (Logging.debugLogging) {
            // TODO: Move to request module.
            Logging.log_debug_msg(
                "Model.startRequest",
                "Started request: " + name + "\n" +
                    "Request Data:\n" + JSON.stringify(request)
            );
        }
    }
};

var ViewPrototype = {
    /*
     * The document is ready, do any dynamic construction.
     * 
     * (but note that we can't talk to the ViewModel yet).
     */
    setup: function () {
        "use strict";
        Application.viewCreated();
    },
    
    /********************************************************************
     *                         Notifications
     ********************************************************************/
    
    sendErrorNotification: function (msg) {
        "use strict";
        alertify.error(msg);
    },
    
    /*
     * Display a message to the user, and ask them to make a choice between 
     * two options
     */
    getUserYesNoChoice: function (msg, yesChoice, noChoice, fn) {
        "use strict";
        alertify.set({labels : {
            ok: yesChoice,
            cancel: noChoice
        }});
        alertify.confirm(msg, fn);
    },

    /********************************************************************
     *                         Utility Methods
     ********************************************************************/

     getScreenWidth: function () {
         "use strict";
         return $(window).width();
     },

     getScreenHeight: function () {
         "use strict";
         return $(window).height();
     },
};

var PresenterPrototpye = {
    /*
     * The model is ready, and the view has been built.
     *
     * Update the view based on the initial state of the application, and then
     * start the application
     */
    start: function () {
        "use strict";
        // Nothing to do...
    }
};


/*
 * Extend an object with the properties of another object
 * 
 *  destination: The target to extend
 *  source: The object to copy methods from
 */
function extend(destination, source) {
    "use strict";
    var k = null;
    for (k in source) {
        if (source.hasOwnProperty(k)) {
            destination[k] = source[k];
        }
    }
    return destination;
}


Application = {
    // We can't start the app until both are ready...
    modelSetupComplete: false,
    viewSetupComplete: false,
    
    model: {},
    view: {},
    presenter: {},
    
    constructor: function () {
        "use strict";
       
        extend(this.presenter, PresenterPrototpye);
        extend(this.model, ModelPrototype);
        extend(this.view, ViewPrototype);
    
    },
    
    /*
     * Initialise the application. This happens in two phases:
     *  
     *  1. Model Initialisation & View Construction
     *  2. Presenter Initialisation (Starts the application)
     *
     *
     *  Model Initialisation:
     *     The model is given a notification that the document is ready, and that
     *     it should do any static initialisation that the view will require to 
     *     to construct itself.
     *
     *     The application now awaits a call to ModelReady(). Once the ModelReady()
     *     has been called the model should be ready to handle a call to interogate it.
     *
     *     At this point the view has not been constructed and it may not make a call
     *     to the view
     *
     *  View Construction: 
     *     The document is ready, but the model is not yet ready for interogation 
     *     (we do not have a ViewModel). The view should do any static initialisation
     *     and then call ViewReady() once it has completed setup.
     *
     *  Presenter Initialisation:
     *     The model is ready, and the view has been built. The presenter should now
     *     do any initialisation based on the model and then start the application
     *
     *  
     */
    start: function () {
        "use strict";
        this.view.setup();
        this.model.initialise();
        
    },
    
    /*
     * Call-back used by the view layer to noitfy us that it has finished construction
     */
    viewCreated: function () {
        "use strict";
        console.log("View has declared itself ready!");
        Application.viewSetupComplete = true;
        Application.tryApplicationStart();
    },
    
    /*
     * Call-back used by the model to notify us that it is ready to handle 
     * requests.
     */
    modelReady: function () {
        "use strict";
        console.log("Model has declared itself ready!");
        Application.modelSetupComplete = true;
        Application.tryApplicationStart();
    },
    
    /* INTERNAL
     * Try to start the application ( this will fail if either the model or the 
     *  view have not responded yet).
     */
    tryApplicationStart: function () {
        "use strict";
        if (Application.viewSetupComplete && Application.modelSetupComplete) {
            Application.applicationStart();
        }
            
    },
    
    /* INTERNAL
     * Internal function used to start the application (shoudl 
     * only be called once the view and model are ready).
     */
    applicationStart: function () {
        "use strict";
        console.log("Starting Application...");
        Application.presenter.start();
    },
    /********************************************************************
     *                         Timmers
     ********************************************************************/
    /*
     * Do an action in <timems> mili-seconds time
     */
    delayAction: function (action, timems) {
        "use strict";
        return window.setTimeout(action, timems);
    }
};


/*
 * Start the spinner
 */
function loadSpinner() {
    "use strict";
    var spinner = new Spinner().spin();
    $(".loading-spinner").html(spinner.el);
}

/*
 * Raise an alert
 */
function alert() {
    "use strict";
    alertify.alert("Sorry, I couldn't load the file - it does not appear to be a valid callgrind file!");
}

/*
 * Notify the user of an error
 */
function logError() {
    "use strict";
    alertify.error("Failed to load callgrind.out");
}

Application.constructor();
/*
 * Prepare the page:
 *  - Setup the analysis menu so it can flip modes
 */
$(document).ready(function () {
    "use strict";
    Logging.releaseMode();
    Select.releaseMode();
    Application.start();
});
