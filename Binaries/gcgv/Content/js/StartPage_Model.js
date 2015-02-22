
/*global $, document, alert, Application, console */

/*
 * Code for interacting with the C++ Application before loading a file
 */
var StartPage = {
    
    /********************************************************************
     *                    Model Interface
     *   Implement methods on the model interface
     ********************************************************************/
    /*
     * Setup the model
     */
    initialise: function () {
        "use strict";
        Application.modelReady();
    },
    
    /********************************************************************
     *                    File to Load
     ********************************************************************/
    fname: "",
    
    getCurrentFile: function () {
        "use strict";
        return this.fname;
    },
    
    /*
     * Start loading a new file, if a file is currently loading that will 
     * be cancelled, and then a new load will begin.
     *
     * The presenter is notified when the load is complete, and in the 
     * event of a failure a reason.
     */
    loadCallgrindFile: function (fname) {
        "use strict";
        Application.delayAction(
            function () {
                Application.presenter.loadFailed(fname, "C++ Interface not yet ready");
            },
            500
        );
    }
};