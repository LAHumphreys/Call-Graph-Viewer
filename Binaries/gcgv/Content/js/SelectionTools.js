/*global $, document, navbar, application, extend, console, Logging*/

var Select = {
    select: null,
    
    debugMode: function () {
        "use strict";
        this.select = this.debugSelect;
    },
    
    releaseMode: function () {
        "use strict";
        this.select = this.releaseSelect;
    },
    
    debugSelect: function (name, pattern) {
        "use strict";
        var items = null, msg = "";
        try {
            items = $(pattern);

            Logging.logElement(name, pattern, items);
        } catch (err) {
            // Log a useful warning...
            msg = "";
            msg += "SELECTOR " + name + " FAILED: " + pattern + "\n";
            msg += "ERROR: " + err;
            Logging.warning("Select.debugSelect", msg);
            // Now re-throw..
            throw err;
        }
        return items;
    },
    
    releaseSelect: function (name, pattern) {
        "use strict";
        return $(pattern);
    },
    
    selectExactly: function (name, pattern, toGet) {
        "use strict";
        var items = null, msg = "";
        try {
            items = this.select(name, pattern);

            if (items.length !== toGet) {
                Logging.logElement(name, pattern, items, "WARNING");
                throw "INVALID SELECTOR (" + pattern + ")"
                      + ": expected " + toGet
                      + " elevements, but got " + items.length;
            }
        } catch (err) {
            // Log a useful warning...
            msg = "";
            msg += "SELECTOR " + name + " FAILED: " + pattern + "\n";
            msg += "ERROR: " + err;
            Logging.warning("Select.selectExactly", msg);
            // Now re-throw..
            throw err;
        }
        return items;
    }
};