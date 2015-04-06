/*global $, document, alert, Application, console, Logging */

/*
 * Code for interacting with the analysis data-model
 */
var Analyse = {
    /********************************************************************
     *                    Model Interface
     *   Implement methods on the model interface
     ********************************************************************/
    /*
     * Setup the model
     */
    initialise: function () {
        "use strict";
        this.initialiseAnalysisMode();
        
        this.startRequest(
            "GetGraphDetails",
            {
            },
            this.detailsHandler,
            function (code, msg) {
                Logging.warning(
                    "Analyse_Initialise",
                    "FATAL ERROR: Failed to get graph details:\n" + msg
                );
            }
        );
    },
    
    detailsHandler: function (details) {
        "use strict";
        var self;
        self = Application.model;
        self.fileName = details.fileName;
        
        self.initialiseDataTypes(details);
        
        Logging.log_debug_msg(
            "Analyse_Model.detailsHandler",
            "Found " + details.units.length + " units"
        );
        
        Application.model.getFlatView();
        
        Application.modelReady();
    },
    
    /********************************************************************
     *                    Callgrind file 
     ********************************************************************/
    
    fileName: "",
    
    getFileName: function () {
        "use strict";
        return this.fileName;
    },
    
    /********************************************************************
     *                    depth 
     ********************************************************************/
    
    depth: 1,
    
    getDepth: function () {
        "use strict";
        return this.depth;
    },
    
    setDepth: function (depth) {
        "use strict";
        this.depth = depth;
        Application.presenter.depthChanged(this.depth);
        
        this.getFlatView();
    },
    
    /********************************************************************
     *                    search Text 
     ********************************************************************/
    filter: null,
    
    clearFilter: function () {
        "use strict";
        var updateRequired = false;
        if (this.filter) {
            this.filter = null;
            updateRequired = true;
        }
        
        if (updateRequired) {
            this.getFlatView();
        }
    },
    
    setFilter: function (pattern) {
        "use strict";
        this.filter = null;
        
        var updateRequired = false;
        if (this.filter) {
            if (this.filter !== pattern) {
                this.filter = pattern;
                updateRequired = true;
            } // else no update required
        } else {
            updateRequired = true;
            this.filter = pattern;
        }
        
        if (updateRequired) {
            this.getFlatView();
        }
    },
    
    getFilter: function () {
        "use strict";
        if (this.filter) {
            return this.filter;
        } else {
            return "";
        }
    },
    
    /********************************************************************
     *                    Request table data 
     ********************************************************************/
    
    currentFlatSearch: null,
    nextFlatSearch: null,
    
    /**
     * Entry point for an external party wishing to update the flat view with new data.
     *
     * If there is no on-going query we can schedule it immediately. If there is an 
     * ongoing query, queue the new query (cancelling any currently waiting to be 
     * dispatched).
     */
    getFlatView: function () {
        "use strict";
        var filter;
        
        this.nextFlatSearch = {
            depth: this.getDepth(),
            pageSize: 25,
            sortMethod: "Total Time"
        };
        
        filter = this.getFilter();
        if (filter !== "") {
            this.nextFlatSearch.filter = filter;
        }
        
        this.getNextFlatView();
    },
    
    /*
     * Manages our queue
     */
    getNextFlatView: function () {
        "use strict";
        
        if (this.nextFlatSearch) {
            if (this.currentFlatSearch) {
                Logging.log_debug_msg(
                    "Analyse_Model.getNextFlatView",
                    "Ongoing search, cannot re-schedule"
                );
            } else {
                this.startFlatViewSearch();
            }
        } else {
            Logging.log_debug_msg(
                "Analyse_Model.getNextFlatView",
                "No next search to schedule"
            );
        }
    },
    
    /*
     * Dispatch the query
     */
    startFlatViewSearch: function () {
        "use strict";
        this.currentFlatSearch = this.nextFlatSearch;
        this.nextFlatSearch = null;
        
        Logging.log_debug_msg(
            "AnalyseModel.getFlatView",
            "Sending Request"
        );
        
        this.startRequest(
            "GetFlatView",
            this.currentFlatSearch,
            this.flatViewHandler,
            this.flatViewError
        );
    },
    
    /*
     * Handle a query reject, and flush the queue
     */
    flatViewError: function (code, msg) {
        "use strict";
        Logging.warning(
            "AnalyseModel.FlatViewFailed",
            "Failed to get flat view data: " + msg
        );
        
        Application.presenter.dataLoadFailed("Flat View", msg);
        
        var self;
        
        self = Application.model;
        
        self.currentFlatSearch = null;
        self.getNextFlatView();
    },
    
    /*
     * Handle query data, and flush the results
     */
    flatViewHandler: function (response) {
        "use strict";
        var data, self;
        Logging.log_debug_msg(
            "AnalyseModel.FlatViewData",
            "Got " + response.data.length.toString() + " rows of data"
        );
        Application.presenter.setFlatViewData(response.data);
        
        self = Application.model;
        
        self.currentFlatSearch = null;
        self.getNextFlatView();
    },

    /********************************************************************
     *                    Analysis Mode
     *  The analysis mode switches our view on the data. (By default we 
     *  can view the data either as raw sum, or as the average time per 
     *  call)
     ********************************************************************/
    analysisMode: "",
    
    initialiseAnalysisMode: function () {
        "use strict";
        this.analysisMode = "Total Time in Function";
    },
    
    getAnalysisMode: function () {
        "use strict";
        return this.analysisMode;
    },
    
    setAnalysisMode: function (mode) {
        "use strict";
        this.analysisMode = mode;
        Application.presenter.analysisModeUpdated(mode);
    },
    
    /********************************************************************
     *                    Supported Data Types
     * The set of data-types we support using. This
     *  will change based on the underlying data we started with.
     ********************************************************************/
    
    /*
     * This object represents the list of data-types we support.
     * 
     * It will be initialised by initialiseDataTypes so that the id array
     * points to an object of the form {name, type, id}, and the types 
     * is a hierachical map:
     *
     * dataTypes = {
     *   category1: {
     *      type1: id1,
     *      type2: id2
     *   },
     *   category2: {
     *      type3: id3,
     *      type4: id4
     *   },
     * }
     */
    dataTypes: {
        
        types: { },
        ids: { },
        
        DataType: {
            name: "",
            type: "",
            id: "",
            
            New: function (name, type, id) {
                "use strict";
                return { name: name, type: type, id: id};
            }
        },
        
        add: function (name, type, id) {
            "use strict";
            var added = false, ptype = null;
            
            if (this.ids.hasOwnProperty(id)) {
                console.log("Duplicate Id: " + id);
            } else {
                if (this.types.hasOwnProperty(type)) {
                    ptype = this.types[type];
                } else {
                    this.types[type] = { };
                    ptype = this.types[type];
                    this[name] = this.DataType.New(name, type);
                }

                if (ptype.hasOwnProperty(name)) {
                    console.log("Duplicate Name! " + name + ", on " + type);
                } else {
                    ptype[name] = id;
                    this.ids[id] = { name: name, type: type, id: id };
                }
            }
        }
    },
        
    /*
     * Return a copy fo the -types available on the model. 
     */
    getDataTypes: function () {
        "use strict";
        return $.extend(true, {}, this.dataTypes.types);
    },
    
    addDataType: function (code) {
        "use strict";
        if (code === "Ir") {
            this.dataTypes.add("Total instructions", "Basic Performance", "Ir");
        } else if (code === "Dr") {
            this.dataTypes.add("Total memory reads", "Basic Performance", "Dr");
        } else if (code === "Dw") {
            this.dataTypes.add("Total memory writes", "Basic Performance", "Dw");
        } else if (code === "DLmr") {
            this.dataTypes.add(
                "Read misses (last line)",
                "Cache Performance (Cache misses)",
                "DLmr"
            );
        } else if (code === "DLmw") {
            this.dataTypes.add(
                "Write misses (last line)",
                "Cache Performance (Cache misses)",
                "DLmw"
            );
        } else if (code === "ILmr") {
            this.dataTypes.add(
                "Instruction misses (last line)",
                "Cache Performance (Cache misses)",
                "ILmr"
            );
        } else if (code === "D1mr") {
            this.dataTypes.add(
                "Read misses (first line)",
                "First Line Cache Performance (L1 Cache misses)",
                "D1mr"
            );
        } else if (code === "D1mw") {
            this.dataTypes.add(
                "Write misses (first line)",
                "First Line Cache Performance (L1 Cache misses)",
                "D1mw"
            );
        } else if (code === "I1mr") {
            this.dataTypes.add(
                "Instruction misses (first line)",
                "First Line Cache Performance (L1 Cache misses)",
                "I1mr"
            );
        } else {
            this.dataTypes.add(code, "Other", code);
        }
    },
    
    initialiseDataTypes: function (details) {
        "use strict";
        var i;
        
        for (i = 0; i < details.units.length; i += 1) {
            this.addDataType(details.units[i]);
        }
        
        this.dataType = this.dataTypes.ids.Ir;
        // TODO: One day this will involve a c++ invocation...
    },
    
    /********************************************************************
     *                    Active Data Type
     *  The data type is the actual unit of data we are analysing. 
     ********************************************************************/
    
    dataType: null,
    
    /*
     * Return an object of the form {
     *   name: <name>
     *   type: <type>
     *   id: <id>
     * }
     */
    getDataType: function () {
        "use strict";
        return $.extend(true, {}, this.dataType);
    },
    
    setDataType: function (id) {
        "use strict";
        var ok = false;
        if (this.dataTypes.ids.hasOwnProperty(id)) {
            this.dataType = this.dataTypes.ids[id];
            ok = true;
        } else {
            console.log(this.dataTypes);
            console.log("Invalid ID: " + id);
        }
        
        if (ok) {
            Application.presenter.dataTypeUpdated(
                this.getDataType()
            );
        }
        return ok;
    }
};