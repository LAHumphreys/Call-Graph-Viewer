
/*global $, document, alert, Application, console, Logging */

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
     *                    File suggestions
     ********************************************************************/
    
    matchingFiles: [],
    currentPattern: null,
    nextPattern: null,
    
    getMatchingFiles: function () {
        "use strict";
        return this.matchingFiles;
    },
    
    updateMatchingFiles: function (pattern) {
        "use strict";
        if (this.currentPattern) {
            // a get currently in progress...
            if (pattern !== this.currentPattern) {
                this.nextPattern = pattern;
            }
        } else {
            this.nextPattern = pattern;
        }
        
        this.startNextFileRequest();
    },
    
    startNextFileRequest: function () {
        "use strict";
        
        if (!this.currentPattern && this.nextPattern) {
            this.currentPattern = this.nextPattern;
            this.nextPattern = null;
            
            this.startRequest(
                "FileList",
                {
                    pattern: this.currentPattern
                },
                this.onMatchingFilesSuccess,
                this.onMatchingFileError
            );
            Logging.log_debug_msg(
                "StartPage_Model.onMatchingFileSuccess",
                "Started a new request!"
            );
        } else {
            Logging.log_debug_msg(
                "StartPage_Model.onMatchingFileSuccess",
                "No more requests to do..."
            );
        }
    },
    
    onMatchingFilesSuccess: function (response) {
        "use strict";
        var self, msg, len, i;
        self = Application.model;
        
        self.matchingFiles = response.files;
        Application.presenter.fileListChanged();
        
        if (Logging.debugLogging) {
            msg =  "Completed get of file list for: " + self.currentPattern;
            msg += "\nGot: \n";
            len = self.getMatchingFiles().length;
            for (i = 0; i < len; i += 1) {
                msg += self.getMatchingFiles()[i] + "\n";
            }
                
            Logging.log_debug_msg(
                "StartPage_Model.onMatchingFileSuccess",
                msg
            );
        }
        self.currentPattern = null;
        self.startNextFileRequest();
    },
    
    onMatchingFileError: function (code, reason) {
        "use strict";
        
        var self, msg = "Failed to get pattern: " + this.currentPattern +
            "\nbecause: \n" + reason;
        
        self = Application.model;
        
        Logging.warning("StartPage_Model.onMatchingFileError", msg);
        
        self.currentPattern = null;
        self.startNextFileRequest();
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
        this.startRequest(
            "LoadGraph",
            {
                file: fname
            },
            function (result) {
                window.location.href = "Analyse.html";
            },
            function (code, msg) {
                Application.presenter.loadFailed(fname, msg);
            }
        );
        Logging.log_debug_msg(
            "StartPage_Model.loadCallgrindfile",
            "Started a new load request!"
        );
    }
    
};