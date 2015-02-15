/*global $, document, alert, Analyse, Application, console */

var NavBar = {
    navbarReady: false,
    /********************************************************************
     *                         Installation
     ********************************************************************/
    /*
     * Load the navbar component
     */
    installNavbar: function () {
        "use strict";
        console.log("install navbar...");
        $(".navbar-placeholder").load("Analyse_Navbar.html", this.initialiseNavbar);
    },
    
    
    /*
     * Initialise any java-script hooks onto the nav-bar components after
     * they have been loaded.
     */
    initialiseNavbar: function () {
        "use strict";
        var self = Application.view;
        
        self.setupAnalysisModeMenu();
        
        // We're done - Notify the view
        self.navbarReady = true;
        self.componentDone();
    },
    /********************************************************************
     *                    Analysis Mode
     *  The analysis mode switches our view on the data. (By default we 
     *  can view the data either as raw sum, or as the average time per 
     *  call)
     ********************************************************************/

    /*
     * Prepare hooks for the analyis drop down
     */
    setupAnalysisModeMenu: function () {
        "use strict";

        $(".analysis-mode-selctor li a").click(function () {
            Analyse.setAnalysisMode($(this).text());
        });
    },

    /*
     * Update the view after a new analysis mode has been chossen
     */
    setAnalysisMode: function (mode) {
        "use strict";
        $(".analysis-mode-label").text(mode);
    },
    
    
    /********************************************************************
     *                         Data Type
     *  The data type switches the underlying data type being used.
     *  This data is dynamic and would depend entirely on the data types 
     *  supported by the model
     ********************************************************************/
    
    /*
     * Prepare hooks for the analyis drop down
     *   dataTypes: An object with an item for each supported category. Each item
     *              has an item for each name of dataType in the category. This 
     *              name-item is a string containing the id to pass to the 
     *              presenter in the event of a change.
     */
    setupDataType: function (dataTypes) {
        "use strict";
        var cat = null, name = null, category = null, id = null;
        for (cat in dataTypes) {
            if (dataTypes.hasOwnProperty(cat)) {
                category = dataTypes[cat];
                this.appendDataTypeCategory(cat);
                for (name in category) {
                    if (category.hasOwnProperty(name)) {
                        id = category[name];
                        this.appendDataType(name, id);
                    }
                }
            }
        }
    },
    
    /* INTERNAL
     * Append a new data-type to the end of the data-types menu. 
     *
     * NOTE: This will obviously be added to whichever is the last category,
     *       so should only be called by setupDataType
     */
    appendDataType: function (name, id) {
        "use strict";
        var item = null;
        item = $("<li><a href='#'> Simple Table </a></li>")
            .children("a")
                .text(name)
            .end()
            .attr("data-id", id)
            .click(function () {
                var id = "";
                id = $(this).attr("data-id");
                Application.presenter.setDataType(id);
            });
        $("ul.data-selector").append(item);
    },
    
        /* INTERNAL
     * Append a new data-type category to the end of the data-types menu. 
     *
     */
    appendDataTypeCategory: function (name) {
        "use strict";
        var item = null, divider = null, selector = null;
        divider = $("<li class='divider'>");
        item = $("</li><li class='dropdown-header'>Data Type</li>").text(name);
        selector = $("ul.data-selector");
        if (selector.children().length > 0) {
            selector.append(divider);
        }
        selector.append(item);
    },
    
    /*
     * Update the select Data Type
     */
    setDataType: function (name) {
        "use strict";
        console.log("Updating: " + name.name);
        $("span.data-model-label").text(name.name);
    }
    
    /********************************************************************
     *                    Visualisation Mode
     ********************************************************************/

    /********************************************************************
     *                         View Switches
     ********************************************************************/

};