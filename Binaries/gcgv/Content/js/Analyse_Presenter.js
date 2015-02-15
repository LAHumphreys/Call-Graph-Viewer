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
    }
};