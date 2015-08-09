/*global $, document, Application, Logging */

/*
 * The presenter is responsible for handling "actions" by the user, 
 * and apply their resulting changes to the model.
 */
var Analyse_Presenter = {
    
    visualisationMode: null,
    
    /********************************************************************
     *                         Installation
     ********************************************************************/
    
    start: function () {
        "use strict";
        this.analysisModeUpdated(Application.model.getAnalysisMode());
        Application.view.setupDataType(Application.model.getDataTypes());
        Application.view.setDataType(Application.model.getDataType());
        Application.view.setFilename(Application.model.getFileName());
        Application.view.setDepth(Application.model.getDepth());
        this.switchToTableVisualisation();
    },
    
    /********************************************************************
     *                         Analysis Mode
     *******************************************************************/
    
    /*
     * Update the analysis used by the model. 
     *
     * This should trigger a call to analysisModeUpdated from the model
     */
    setAnalysisMode: function (newMode) {
        "use strict";
        Application.model.setAnalysisMode(newMode);
    },
    
    analysisModeUpdated: function (newMode) {
        "use strict";
        Application.view.setAnalysisMode(newMode);
    },
    
        
    /********************************************************************
     *                         Data Type
     *******************************************************************/
    
    setDataType: function (id) {
        "use strict";
        Application.model.setDataType(id);
    },
    
    dataTypeUpdated: function (name) {
        "use strict";
        Application.view.setDataType(name);
    },
    
    /********************************************************************
     *                         Depth Controller
     *******************************************************************/
    
    /**
     * Called by the view to notify us that the user has updated the depth controller.
     */
    updateDepth: function (depth) {
        "use strict";
        Application.model.setDepth(depth);
    },
    
    depthChanged: function (depth) {
        "use strict";
        Application.view.setDepth(depth);
    },
    
    /********************************************************************
     *                         Filter
     *******************************************************************/
    
    /**
     * Called by the view to notify us that the user has updated the filter
     */
    updateFilter: function (filter) {
        "use strict";
        Application.model.setFilter(filter);
    },
    
    /********************************************************************
     *                         Bread Crumbs
     *******************************************************************/
    
    /**
     * Update the present working directory
     */
    pwdChanged: function (pwd) {
        "use strict";
        Application.view.clearPWD();
        
        var i = 0, path = "", node = "";
        for (i = 0; i < pwd.length; i += 1) {
            node = pwd[i];
            if (i > 0) {
                path += "/";
            }
            path += node;
            Application.view.addChildNodeToPWD(node, path);
        }
        Logging.log_debug_msg(
            "Analyse_Presenter.pwdChanged",
            "Updated Current working directory to: " + path
        );
    },
    
    nodeSelected: function (path) {
        "use strict";
        Application.model.gotoNode(path);
        Application.view.closeFinder();
    },
    
    /********************************************************************
     *                         Flat View
     * Various flat-view visualisations:
     *    o A simple table view of the data
     *    o A pie chart of the data
     *******************************************************************/
    
    setFlatViewData: function (rows) {
        "use strict";
        var i, data, row, shortName;
        Application.view.clearData();
        if (this.visualisationMode === "Simple Table") {
            for (i = 0; i < rows.length; i += 1) {
                Application.view.addRow(rows[i]);
            }
        } else if (this.visualisationMode === "Pie Chart") {
            data = [];
            for (i = 0; i < rows.length; i += 1) {
                row = rows[i];
                if (row.name.length > 60) {
                    shortName = row.name.substr(0, 57) + "...";
                } else {
                    shortName = row.name;
                }
                data.push(
                    {
                        name: row.name,
                        short_name: shortName,
                        y: row.total
                    }
                );
            }
            Application.view.setPieData(data);
        } else {
            Application.view.sendErrorNotification(
                this.visualisationMode + " is not supported yet!"
            );
        }
    },
    
    setGraphViewData: function (rows) {
        "use strict";
        if (this.visualisationMode === "Call Graph") {
            Application.view.setTreeData(rows);
        } else if (this.visualisationMode === "Radial Pie Chart") {
            Application.view.setRadialData(rows);
        } else {
            Application.view.sendErrorNotification(
                this.visualisationMode + " is not supported yet!"
            );
        }
    },
    
    /*
     * We failed to get the data for this row 
     */
    dataLoadFailed: function (type, reason) {
        "use strict";
        Application.view.sendErrorNotification(
            "Failed to load '" + type + "':<br> &nbsp;&nbsp;&nbsp;&nbsp;   " + reason
        );
        Application.view.clearData();
    },
    
    switchToTableVisualisation: function () {
        "use strict";
        this.visualisationMode = "Simple Table";
        Application.view.setActiveVisualisation(this.visualisationMode);
        Application.view.hidePieChart();
        Application.view.hideCallGraph();
        Application.view.showTable();
        Application.view.hideRadial();
        Application.model.renotifyFlatViewData();
    },
    
    switchToPieChartVisualisation: function () {
        "use strict";
        this.visualisationMode = "Pie Chart";
        Application.view.setActiveVisualisation(this.visualisationMode);
        Application.view.hideTable();
        Application.view.hideCallGraph();
        Application.view.showPieChart();
        Application.view.hideRadial();
        Application.model.renotifyFlatViewData();
    },
    
    switchToCallGraphVisualisation: function () {
        "use strict";
        this.visualisationMode = "Call Graph";
        Application.view.setActiveVisualisation(this.visualisationMode);
        Application.view.hideTable();
        Application.view.showCallGraph();
        Application.view.hidePieChart();
        Application.view.hideRadial();
        Application.model.renotifyGraphViewData();
    },
    
    switchToRadialVisualisation: function () {
        "use strict";
        this.visualisationMode = "Radial Pie Chart";
        Application.view.setActiveVisualisation(this.visualisationMode);
        Application.view.hideTable();
        Application.view.hideCallGraph();
        Application.view.hidePieChart();
        Application.view.showRadial();
        Application.model.renotifyGraphViewData();
    },
    
    /********************************************************************
     *                         Finder 
     *******************************************************************/
    
    rowSelected: function (name) {
        "use strict";
        Application.model.findNode(name);
    },
    
    setFinderData: function (rows) {
        "use strict";
        var i;
        if (rows.length === 1) {
            Application.model.gotoNode(rows[0].path);
        } else {
            Logging.log_debug_msg(
                "Fidner.addFinderRows",
                "Clearing Finder..."
            );
            Application.view.clearFinder();
            Logging.log_debug_msg(
                "Fidner.addFinderRows",
                "Adding Rows..."
            );
            Application.view.addFinderRows(rows);
            Logging.log_debug_msg(
                "Fidner.addFinderRows",
                "Opening Finder..."
            );
            Application.view.openFinder();
            Logging.log_debug_msg(
                "Fidner.addFinderRows",
                "done.."
            );
        }
    },
    
    nodeFindFailed: function (reason) {
        "use strict";
        Application.view.sendErrorNotification(
            "Failed to find these nodes: " + reason
        );
        Application.view.clearFinder();
        Application.view.closeFinder();
    },
    
    /********************************************************************
     *                         Working Node 
     *******************************************************************/
    /*
     * We failed to change node... 
     */
    nodeChangeFailed: function (reason) {
        "use strict";
        Application.view.sendErrorNotification(
            "Cannot open this node: " + reason
        );
        Application.view.clearData();
    }
};