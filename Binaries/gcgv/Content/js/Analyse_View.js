/*global $, document, NavBar, Application, extend, console, self */

var Analyse_View = {
    /*
     * Build the view...
     */
    setup: function () {
        "use strict";
        extend(this, NavBar);
        this.installNavbar();
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