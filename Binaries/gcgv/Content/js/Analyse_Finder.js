/*global $, document, NavBar, Application, extend, console, self, Select, Logging */

var Finder = {
    finderReady: false,

    title: "Node Finder",

    /********************************************************************
     *                         Accessors
     ********************************************************************/
    
    getPlaceholder: function () {
        "use strict";
        return Select.selectExactly("Analyse View - Finder Container", ".Analyse-Finder-Placeholder", 1);
    },

    getFinderTable: function () {
        "use strict";
        return Select.selectExactly("Analyse View - Finder Table", "#finderTable table", 1);
    },
    
    getFinderTableBody: function () {
        "use strict";
        return Select.selectExactly("Analyse View - Finder Table", "#finderTable table tbody", 1);
    },
    
    getSelectedRow: function () {
        "use strict";
        return this.getFinderTableBody().children().filter("tr.info");
    },
    /********************************************************************
     *                         Installation
     ********************************************************************/
    /*
     * Load the finder component
     */
    installFinder: function () {
        "use strict";
        this.getPlaceholder().load("Analyse_Finder.html", this.initialiseFinder);
    },
     
    /*
     * Initialise any java-script hooks onto the nav-bar components after
     * they have been loaded.
     */
    initialiseFinder: function () {
        "use strict";
        var self = Application.view;
        
        self.getPlaceholder().dialog({
            autoOpen: false,
            modal: true,
            title: self.title,
            width: 0.85 * self.getScreenWidth(),
            height: 0.75 * self.getScreenHeight(),
            buttons: [
                {
                    text: "Cancel",
                    click: function () {
                        Application.view.closeFinder();
                    }
                },
                {
                    text: "Go to Node",
                    click: function () {
                        Application.view.finderGotoSelectedNode();
                    }
                }
            ]
        });

        // We're done - Notify the view
        self.finderReady = true;
        self.componentDone();
    },

    openFinder: function () {
        "use strict";
        this.getPlaceholder().dialog("open");
    },
    
    closeFinder: function () {
        "use strict";
        this.getPlaceholder().dialog("close");
    },
    
    finderGotoSelectedNode: function () {
        "use strict";
        var row = null, path = "";
        row = this.getSelectedRow();
        if (row.length === 1) {
            path = row.attr("data-path");
            Application.presenter.nodeSelected(path);
        } else {
            Application.view.sendErrorNotification("Select a node first!");
        }
    },
    
    clearFinder: function () {
        "use strict";
        this.getFinderTableBody().empty();
    },
    
    clearFinderSelectedRow: function () {
        "use strict";
        var cell = null, row = this.getSelectedRow();
        if (row.length > 0) {
            cell = row.children().filter("td:first");
            cell.empty();
            cell.append(row.attr("data-path").replace(/\//g, "/ "));
        }
        row.removeClass("info");
    },
    
    expandRow: function (row) {
        "use strict";
        var cell = null, content = $("<ol></ol>"), i = 0, nodes = row.attr("data-path").split("/");
        
        for (i = 0; i < nodes.length; i += 1) {
            content.append($("<li>" + nodes[i] + "</li>"));
        }
        
        cell = row.children().filter("td:first");
        
        cell.empty();
        cell.append(content);
        
        row.addClass("info");
    },
    
    addFinderRows: function (rows) {
        "use strict";
        var i = 0, html_rows = "", row = null;
        for (i = 0; i < rows.length && i < 25; i += 1) {
            row = rows[i];
            html_rows += "<tr data-path='" + row.path + "'>" +
                "<td>" + row.path.replace(/\//g, "/ ") + "</td>" +
                "<td>" + row.calls.toString() + "</td>" +
                "<td>" + row.cost.toString() + "</td>" +
                "<td>" + row.avCost.toString() + "</td>" +
                "</tr>";
        }
        
        this.getFinderTableBody().html(html_rows);
        
        this.getFinderTableBody().children().filter("tr").click(function (e) {
            Application.view.clearFinderSelectedRow();
            Application.view.expandRow($(this));
        });
    }
};

