/*global $, document, NavBar, Application, extend, console, self, Select, Finder, Highcharts, Tree, d3 */
/**
 * Luke Humphreys 2015
 *
 * This is a refactored version of the example from d3 demo site:
 *    http://charts.animateddata.co.uk/datavistree/
 *
 *  Cosmetic changes to the code have been made to protect the rest of the
 *  javascript application from the global functions / variables in the
 *  original. Some of the more obscure variables have been renamed (particuarly
 *  the mysterious m array), and expressions have been simplified where
 *  possible. The logic itself has been kept pretty must as it was found - and
 *  no credit for, nor responsibiltiy for, the correct functioning of this code
 *  is accepted by the author of this file.
 *
 *  The original was created by "Peter Cook" at "Data Visualisation Brighton",
 *  basd on Mike Bostock's example tree layout:
 *      http://mbostock.github.io/d3/talk/20111018/tree.html
 *
 */
function Tree(data, selector) {
    "use strict";
    /****************************************************************
     *                 Initialise the private data...
     ***************************************************************/
    this.margin = {
        top: 20,
        left: 420,
        bottom: 20,
        right: 20
    };
     
    this.selector = selector;

    /**
     * The full height / width off the visualisation on the screen
     */
    this.height = 1000;
    this.width = Application.view.getScreenWidth();

    /*
     * The (smaller) height and width of the tree (after margin has been taken
     * into account)
     */
    this.w = this.width - this.margin.left  - this.margin.right;
    this.h = this.height - this.margin.top - this.margin.bottom;

    this.idCounter = 1;

    this.root = data;
    this.root.x0 = this.h / 2;
    this.root.y0 = 0;

    this.tree = null;

    this.vis = null;

    this.diagonal = d3.svg.diagonal().projection(function (d) {
        return [d.y, d.x];
    });

    this.init(selector);
}
Tree.prototype = {
    /****************************************************************
     *             Initialisation
     ***************************************************************/

    draw: function () {
        "use strict";
        this.update(this.root);
    },
    
    remove: function () {
        "use strict";
        d3.select(this.selector).remove();
    },
    
    /****************************************************************
     *             PRIVATE: Initialisation
     ***************************************************************/

    init: function (selector) {
        "use strict";
        // Initialise a new tree
        this.tree = d3.layout.tree().size([this.h, this.w]);

        // Add the tree onto the provided element
        this.vis = d3.select(selector).append("svg:svg")
                    // Re-size the element to the correct height/ width to
                    // handle the visualisation
                    .attr("width", this.width)
                    .attr("height", this.height)
                  .append("svg:g")
                    // Offset the visualisation within the element to take into
                    // acount our margin
                    .attr("transform", "translate(" + this.margin.right + "," + this.margin.top + ")");
    },

    /****************************************************************
     *             PRIVATE: Graph Actions
     ***************************************************************/

    /**
     * Toggle open / close a node's children
     */
    toggle: function (d) {
        "use strict";
        if (d.children) {
            d._children = d.children;
            d.children = null;
        } else {
            d.children = d._children;
            d._children = null;
        }
    },

    

    /****************************************************************
     *             PRIVATE: Handle an update
     ***************************************************************/
    update: function (source) {
      var THIS = this;
      var duration = 500;

      // Compute the new tree layout.
      var nodes = this.tree.nodes(this.root).reverse();

      // Normalize for fixed-depth.
      nodes.forEach(function(d) { d.y = d.depth * 180; });

      // Update the nodes…
      var node = this.vis.selectAll("g.node")
          .data(nodes, function(d) { return d.id || (d.id = ++THIS.idCounter); });

      // Enter any new nodes at the parent's previous position.
      var nodeEnter = node.enter().append("svg:g")
          .attr("class", "node")
          .attr("transform", function(d) { return "translate(" + source.y0 + "," + source.x0 + ")"; })
          .on("click", function(d) { THIS.toggle(d); THIS.update(d); });

      nodeEnter.append("svg:circle")
          .attr("r", 1e-6)
          .style("fill", function(d) { return d._children ? "lightsteelblue" : "#fff"; });

      nodeEnter.append('a')
          .attr('xlink:href', function(d) {
            return d.url;
          })
          .append("svg:text")
          .attr("x", function(d) { return d.children || d._children ? -10 : 10; })
          .attr("dy", ".35em")
          .attr("text-anchor", function(d) { return d.children || d._children ? "end" : "start"; })
          .text(function(d) { return d.shortName; })
          .style('fill', function(d) {
            return d.free ? 'black' : '#999';
          })
          .style("fill-opacity", 1e-6);

      nodeEnter.append("svg:title")
        .text(function(d) {
          return d.description;
        });

      // Transition nodes to their new position.
      var nodeUpdate = node.transition()
          .duration(duration)
          .attr("transform", function(d) { return "translate(" + d.y + "," + d.x + ")"; });

      nodeUpdate.select("circle")
          .attr("r", 6)
          .style("fill", function(d) { return d._children ? "lightsteelblue" : "#fff"; });

      nodeUpdate.select("text")
          .style("fill-opacity", 1);

      // Transition exiting nodes to the parent's new position.
      var nodeExit = node.exit().transition()
          .duration(duration)
          .attr("transform", function(d) { return "translate(" + source.y + "," + source.x + ")"; })
          .remove();

      nodeExit.select("circle")
          .attr("r", 1e-6);

      nodeExit.select("text")
          .style("fill-opacity", 1e-6);

      // Update the links…
      var link = THIS.vis.selectAll("path.link")
          .data(THIS.tree.links(nodes), function(d) { return d.target.id; });

      // Enter any new links at the parent's previous position.
      link.enter().insert("svg:path", "g")
          .attr("class", "link")
          .attr("d", function(d) {
            var o = {x: source.x0, y: source.y0};
            return THIS.diagonal({source: o, target: o});
          })
        .transition()
          .duration(duration)
          .attr("d", THIS.diagonal);

      // Transition links to their new position.
      link.transition()
          .duration(duration)
          .attr("d", THIS.diagonal);

      // Transition exiting nodes to the parent's new position.
      link.exit().transition()
          .duration(duration)
          .attr("d", function(d) {
            var o = {x: source.x, y: source.y};
            return THIS.diagonal({source: o, target: o});
          })
          .remove();

      // Stash the old positions for transition.
      nodes.forEach(function(d) {
        d.x0 = d.x;
        d.y0 = d.y;
      });
    }

};



