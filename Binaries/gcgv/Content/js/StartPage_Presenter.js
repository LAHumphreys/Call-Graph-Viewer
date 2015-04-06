/*global $, document, NavBar, Application, extend, console, self */

/*
 * The presenter is responsible for handling "actions" by the user, 
 * and apply their resulting changes to the model.
 *
 * It is also responsible for handling the "start" event fired from the application
 */
var Start_Presenter = {
    loading: false,
    /********************************************************************
     *                         Installation
     ********************************************************************/
    
    start: function () {
        "use strict";
        // Application started...
    },
    
    /********************************************************************
     *                         File finding...
     ********************************************************************/
    
    fileListChanged: function () {
        "use strict";
        Application.view.setFileInputSuggestions(
            Application.model.getMatchingFiles()
        );
    },
    
    fileListPatternChanged: function (patt) {
        "use strict";
        Application.model.updateMatchingFiles(patt + "*");
    },
    
    /********************************************************************
     *                         Load Handling...
     ********************************************************************/
    
    startNewLoad: function () {
        "use strict";
        var filename = Application.view.getFilePath();
        Application.view.showSpinner();
        Application.model.loadCallgrindFile(filename);
        this.loading = true;
        //this.scheduleLoadCheck();
    },
    
    stopLoad: function () {
        "use strict";
    },
    
    scheduleLoadCheck: function () {
        "use strict";
        var filename = Application.model.getCurrentFile();
        Application.view.getUserYesNoChoice(filename, "Wait", "Cancel Load");
    },
    
    cancelLoadChoice: function (waitForLoad) {
        "use strict";
        if (waitForLoad) {
            this.scheduleLoadCheck();
        } else {
            this.stopLoad();
        }
    },
    
    /*
     * We failed to load a callgrind file for some reason - notify 
     * the user and reset our state
     */
    loadFailed: function (fname, reason) {
        "use strict";
        Application.view.sendErrorNotification(
            "Failed to load '" + fname + "':<br> &nbsp;&nbsp;&nbsp;&nbsp;   " + reason
        );
        Application.view.hideSpinner();
        this.loading = false;
    }
};