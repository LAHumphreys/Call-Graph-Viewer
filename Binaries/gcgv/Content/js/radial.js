function Radial(data,selector) {
    this.selector = selector;
    
    this.width = Application.view.getScreenWidth();
    this.height = 600;
    this.radius = Math.min(this.width, this.height) / 2;

    this.x = d3.scale.linear().range([0, 2 * Math.PI]);

    this.y = d3.scale.sqrt().range([0, this.radius]);

    this.color = d3.scale.category20c();

    this.svg = d3.select(selector).append("svg")
                    .attr("width", this.width)
                    .attr("height", this.height)
                  .append("g")
                    .attr("transform", "translate(" + this.width / 2 + "," + (this.height / 2 + 10) + ")");

    this.partition = d3.layout.partition()
                        .sort(null)
                        .value(function(d) { return 1; });

    var THIS = this;
    this.arc = d3.svg.arc()
                    .startAngle(function(d) { return Math.max(0, Math.min(2 * Math.PI, THIS.x(d.x))); })
                    .endAngle(function(d) { return Math.max(0, Math.min(2 * Math.PI, THIS.x(d.x + d.dx))); })
                    .innerRadius(function(d) { return Math.max(0, THIS.y(d.y)); })
                    .outerRadius(function(d) { return Math.max(0, THIS.y(d.y + d.dy)); });

    // Keep track of the node that is currently being displayed as the root.
    this.node = null;
    d3.select(self.frameElement).style("height", this.height + "px");

    // TODO: This only works if there is not a second instance...
    this.tooltip = d3.select(selector)
        .append("div")
        .attr("id", "tooltip")
        .style("position", "absolute")
        .style("z-index", "10")
        .style("opacity", 0);
    
    this.load(data)

}

Radial.prototype = {
    /*****************************************
     *           Initialisation / D'tor
     *****************************************/
    
    remove: function () {
        d3.select(this.selector).remove();
    },
    // Setup for switching data: stash the old values for transition.
    stash: function (d) {
        "use strict";
         d.x0 = d.x;
         d.dx0 = d.dx;
    },

    // When switching data: interpolate the arcs in data space.
    arcTweenData: function(a, i) {
        "use strict";
         var oi, THIS;
         THIS = this;
         oi = d3.interpolate({x: a.x0, dx: a.dx0}, a);
         function tween(t) {
            var b = oi(t);
            a.x0 = b.x;
            a.dx0 = b.dx;
            return THIS.arc(b);
         }
         if (i == 0) {
           // If we are on the first arc, adjust the x domain to match the root node
           // at the current zoom level. (We only need to do this once.)
            var xd = d3.interpolate(this.x.domain(), [this.node.x, this.node.x + this.node.dx]);
            return function(t) {
              THIS.x.domain(xd(t));
              return tween(t);
            };
         } else {
            return tween;
         }
    },

    // When zooming: interpolate the scales.
    arcTweenZoom: function (d) {
        "use strict";
          var xd = d3.interpolate(this.x.domain(), [d.x, d.x + d.dx]),
              yd = d3.interpolate(this.y.domain(), [d.y, 1]),
              yr = d3.interpolate(this.y.range(), [d.y ? 20 : 0, this.radius]);
          var THIS = this;
          return function(d, i) {
            return i
                ? function(t) { return THIS.arc(d); }
                : function(t) { THIS.x.domain(xd(t)); THIS.y.domain(yd(t)).range(yr(t)); return THIS.arc(d); };
          };
    },

    load: function(root) {
        "use strict";
         this.node = root;
         var THIS = this;
         var path = this.svg.datum(root).selectAll("path")
              .data(this.partition.nodes)
            .enter().append("path")
              .attr("d", this.arc)
              .style("fill", function(d) { return THIS.color((d.children ? d : d.parent).name); })
              .on("click", click)
              .on("mouseover", function (d) { return THIS.mouseOverArc(d,this); })
              .on("mouseout", function (d) { return THIS.mouseOutArc(this); } )
              .on("mousemove", function (d) { return THIS.mouseMoveArc(d,this); })
              .each(this.stash);

         d3.selectAll("input").on("change", function change() {
            var value = function(d) { return /*d.size*/ 1; };

            path.data(THIS.partition.value(value).nodes)
              .transition()
                .duration(1000)
                .attrTween("d", function (d,i) { return THIS.arcTweenData(d, i);} );
         });

         function click(d) {
            this.node = d;
            path.transition()
              .duration(1000)
              .attrTween("d", function (d) { return THIS.arcTweenZoom(d); });
         }
    },

    /*****************************************
     *           Tool Tip Handling
     *****************************************/

    format_description: function (d) {
      var description = d.description;
          return  '<b>' + d.name + '</b></br>'+ d.description;
    },

    mouseOverArc: function (d, item) {
        d3.select(item).attr("stroke","black")
             
        this.tooltip.html(this.format_description(d));
        return this.tooltip.transition()
            .duration(50)
            .style("opacity", 0.9);
    },

    mouseOutArc: function (item) {
        d3.select(item).attr("stroke","")
        return this.tooltip.style("opacity", 0);
    },

    mouseMoveArc: function (d,item) {
        return this.tooltip
            .style("top", (d3.event.pageY-10)+"px")
            .style("left", (d3.event.pageX+10)+"px");
    }
}
