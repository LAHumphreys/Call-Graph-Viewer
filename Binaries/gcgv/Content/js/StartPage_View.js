/*global $, document, navbar, Select, Application, alertify, Spinner, Logging */

var Start_View = {
    /********************************************************************
     *                         View Interface
     ********************************************************************/
    setup: function () {
        "use strict";
        this.initialiseStartButton();
        this.hideSpinner();
        this.initialiseFileInput();
        Application.viewCreated();
    },
    
    /********************************************************************
     *                         Accessors
     ********************************************************************/
    
    getLoadButton: function () {
        "use strict";
        return Select.selectExactly("Start Page - Load Button", "#startpage-start-button", 1);
    },
    
    getLoadingDiv: function () {
        "use strict";
        return Select.selectExactly("Start Page - Busy Div", "div.loading", 1);
    },
    
    getSpinnerDiv: function () {
        "use strict";
        return Select.selectExactly("Start Page - Busy Div", "div.loading-spinner", 1);
    },
    
    getFileInput: function () {
        "use strict";
        return Select.selectExactly("Start Page - File Input", "#start-page-filepath", 1);
    },
    
    getFilePath: function () {
        "use strict";
        var fname = "";
        fname = this.getFileInput().val();
        
        if (fname === "") {
            fname = "callgrind.out";
        }
        
        return fname;
    },
    
    /********************************************************************
     *                         Loader
     ********************************************************************/
    
    initialiseStartButton: function () {
        "use strict";
        var self = this;
        this.getLoadButton().click(function () {
            Application.presenter.startNewLoad();
        });
    },
    
    initialiseFileInput: function () {
        "use strict";
        var self = this;
        this.getFileInput().keypress(function (e) {
            self.fileInputKeyPressHandler(e);
        });
        
        this.getFileInput().keyup(function (e) {
            Application.presenter.fileListPatternChanged(this.value);
        });
        
        this.setFileInputSuggestions([]);
    },
    
    setFileInputSuggestions: function (suggestions) {
        "use strict";
        this.getFileInput().autocomplete({
            source: suggestions,
            delay: 0
        });
        
        this.getFileInput().autocomplete("search", this.getFileInput().val());
    },
    
    fileInputKeyPressHandler: function (e) {
        "use strict";
        if (e.which === 13) {
            // Enter Key Pressed
            
            // Prevent the form triggering its action because we will handle
            // the result of the form action asynchrounously, causing a redirect 
            // only if necessary
            e.preventDefault();
            
            Application.presenter.startNewLoad();
        }
    },
    
    /********************************************************************
     *                         Spinner...
     ********************************************************************/
    
    spinner: null,
    
    showSpinner: function () {
        "use strict";
        var target = null;
        
        // Show the loading segment...
        this.getLoadingDiv().show();
        
        // Now start the spinner...
        if (this.spinner) {
            this.spinner.spin();
        } else {
            target = this.getSpinnerDiv().get(0);
            this.spinner = new Spinner().spin(target);
        }
    },
    
    hideSpinner: function () {
        "use strict";
        this.getLoadingDiv().hide();
    }
};
