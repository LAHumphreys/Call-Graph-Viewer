/*global $, document, Application */

/*
 * The presenter is responsible for handling "actions" by the user, 
 * and apply their resulting changes to the model.
 */
var Analyse_Presenter = {
    /********************************************************************
     *                         Installation
     ********************************************************************/
    
    start: function () {
        "use strict";
        this.analysisModeUpdated(Application.model.getAnalysisMode());
        Application.view.setupDataType(Application.model.getDataTypes());
        Application.view.setDataType(Application.model.getDataType());
        Application.view.setFilename(Application.model.getFileName());
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
     *                         Flat View
     *******************************************************************/
    
    setFlatViewData: function (rows) {
        "use strict";
        var i;
        Application.view.clearData();
        for (i = 0; i < rows.length; i += 1) {
            Application.view.addRow(rows[i]);
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
    }
    
};