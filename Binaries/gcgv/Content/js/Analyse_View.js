/*global $, document, NavBar, Application, extend, console, self, Select */

var Analyse_View = {
    /*
     * Build the view...
     */
    /********************************************************************
     *                         Initialisation
     ********************************************************************/
    setup: function () {
        "use strict";
        extend(this, NavBar);
        
        this.setupDepthSlider();
        this.setupSearchBox();
        
        this.installNavbar();
    },
    
    setupDepthSlider: function () {
        "use strict";
        // Hook to handle the final selected value
        this.getDepthSlider().change(function (e) {
            Application.presenter.updateDepth(
                parseInt(Application.view.getDepthSlider().val(), 10)
            );
        });
        // Hook to update the label as we slide...
        this.getDepthSlider().on("input", function (e) {
            Application.view.getDepthLabel().text(
                this.value.toString()
            );
        });
    },
    
    /********************************************************************
     *                         Accessors
     ********************************************************************/
    
    getFileNameDisplay: function () {
        "use strict";
        return Select.selectExactly("Analyse - File Name", "#analyse-file-name", 1);
    },
    
    getTableBody: function () {
        "use strict";
        return Select.selectExactly("Flat View Data", "#flatView tbody", 1);
    },
    
    getDepthSlider: function () {
        "use strict";
        return Select.selectExactly("Depth Slider!", "div.depthController input", 1);
    },
    
    getDepthLabel: function () {
        "use strict";
        return Select.selectExactly(
            "Depth Slider",
            "div.depthController span.depthLabel",
            1
        );
    },
    
    getSearchBox: function () {
        "use strict";
        return Select.selectExactly(
            "Search Filter Box",
            "input.searchFilterBox",
            1
        );
    },
    
    /********************************************************************
     *                             Search Box
     ********************************************************************/
    
    setupSearchBox: function () {
        "use strict";
        this.getSearchBox().keyup(function (e) {
            Application.presenter.updateFilter(
                Application.view.getSearchBox().val()
            );
        });
        this.getSearchBox().change(function (e) {
            Application.presenter.updateFilter(
                Application.view.getSearchBox().val()
            );
        });
    },
    
    /********************************************************************
     *                         File name
     ********************************************************************/
    
    setFilename: function (name) {
        "use strict";
        this.getFileNameDisplay().text(name);
    },
    
    /********************************************************************
     *                         Depth Controller
     ********************************************************************/
    
    setDepth: function (depth) {
        "use strict";
        this.getDepthSlider().val(depth);
        this.getDepthLabel().text(depth.toString());
    },
    
    /********************************************************************
     *                         Flat-View Data
     ********************************************************************/
    
    clearData: function () {
        "use strict";
        this.getTableBody().empty();
    },
    
    addRow: function (row) {
        "use strict";
        this.getTableBody().append(
            $("<tr>"
                + "<td>" + row.name + "</td>"
                + "<td>" + row.calls + "</td>"
                + "<td>" + row.total + "</td>"
                + "<td>" + row.average + "</td>" +
                "</tr>")
        );
    },
    
    /*
     * Used by our individual components to signal they are ready
     * 
     * If this is the last component, signal to the application that we 
     * have completed setup
     */
    componentDone: function () {
        "use strict";
        
        if (this.navbarReady) {
            Application.viewCreated();
        }
    }
};