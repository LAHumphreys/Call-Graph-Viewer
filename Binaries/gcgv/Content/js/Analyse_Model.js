/*global $, document, alert, Application, console */

/*
 * Code for interacting with the analysis data-model
 */
var Analyse = {
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
                    console.log("IDs: " + this.ids);
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
    
    initialiseDataTypes: function () {
        "use strict";
        this.dataTypes.add("Total instructions", "Basic Performance", "Ir");
        this.dataTypes.add("Total memory reads", "Basic Performance", "Dr");
        this.dataTypes.add("Total memory writes", "Basic Performance", "Dw");
        
        this.dataTypes.add(
            "Read misses (last line)",
            "Cache Performance (Cache misses)",
            "DLmr"
        );
        this.dataTypes.add(
            "Write misses (last line)",
            "Cache Performance (Cache misses)",
            "DLmw"
        );
        this.dataTypes.add(
            "Instruction misses (last line)",
            "Cache Performance (Cache misses)",
            "ILmr"
        );
        
        this.dataTypes.add(
            "Read misses (first line)",
            "First Line Cache Performance (L1 Cache misses)",
            "D1mr"
        );
        this.dataTypes.add(
            "Write misses (first line)",
            "First Line Cache Performance (L1 Cache misses)",
            "D1mw"
        );
        this.dataTypes.add(
            "Instruction misses (first line)",
            "First Line Cache Performance (L1 Cache misses)",
            "I1mr"
        );
        
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
    },

    /*
     * Setup the model
     */
    initialise: function () {
        "use strict";
        this.initialiseAnalysisMode();
        this.initialiseDataTypes();
        
        Application.modelReady();
    }
};