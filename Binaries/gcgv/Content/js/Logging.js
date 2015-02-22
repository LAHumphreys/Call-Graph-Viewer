/*global $,    console, DEV_TOOLS_Log_Format*/

var Logging = {
    log_element_selection: null,
    log_debug_msg: null,
    
    debugLogging: false,
    
    releaseMode: function () {
        "use strict";
        this.debugLogging = false;
        this.log_element_selection = this.noLog;
        this.log_debug_msg = this.noLog;
    },
    
    debugMode: function () {
        "use strict";
        this.debugLogging = true;
        this.log_element_selection = this.logElement;
        this.log_debug_msg = this.debug;
    },
    
    noLog: function (arg1, arg2) {
        "use strict";
        // Nothing to do...
    },
    
    warning: function (context, msg) {
        "use strict";
        console.log(DEV_TOOLS_Log_Format("WARNING", context, msg));
    },
    
    debug: function (context, msg) {
        "use strict";
        console.log(DEV_TOOLS_Log_Format("DBEUG 1", context, msg));
    },
    
    logElement: function (name, pattern, items, level) {
        "use strict";
        var i = 0, msg = "";
        msg += "Selector: " + pattern + "\n";
        msg += "Items Found: " + items.length + "\n";
        for (i = 0; i < items.length; i += 1) {
            msg += i + ": " + items[i] + "\n";
        }
        for (i = 0; i < items.length; i += 1) {
            msg += "Item " + i + ": " + items[i] + "\n";
            msg += $("<div></div>").html(items.eq(i).clone()).html() + "\n";
        }
        if (!level) {
            if (items.length > 0) {
                level = "DEBUG 1";
            } else {
                level = "WARNING";
            }
        }
        console.log(DEV_TOOLS_Log_Format(level, name, msg));
    }
};