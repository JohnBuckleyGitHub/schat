/*
 *  Easy Tooltip 1.0 - jQuery plugin
 *  written by Alen Grakalic	
 *  http://cssglobe.com/post/4380/easy-tooltip--jquery-plugin
 *
 *  modified for Simple Chat by Alexander Sedov
 *
 *  Copyright (c) 2009 Alen Grakalic (http://cssglobe.com)
 *  Dual licensed under the MIT (MIT-LICENSE.txt)
 *  and GPL (GPL-LICENSE.txt) licenses.
 *
 *  Copyright (c) 2012 Alexander Sedov (http://schat.me)
 *
 *  Built for jQuery library
 *  http://jquery.com
 *
 */

(function($) {

  $.fn.easyTooltip = function(options){

    // default configuration properties
    var defaults = {
      xOffset: 10,
      yOffset: 25,
      tooltipId: "easyTooltip",
      clickRemove: true,
      content: "",
      useElement: ""
    };

    var options = $.extend(defaults, options);
    var content;

    $(this).off("mouseenter.easyTooltip");
    $(this).off("mouseleave.easyTooltip");
    $(this).off("mousemove.easyTooltip");
    $(this).off("mousedown.easyTooltip");

    this.each(function() {
      var title = $(this).attr("title");

      $(this).on("mouseenter.easyTooltip", function(e) {
        content = (options.content != "") ? options.content : title;
        content = (options.useElement != "") ? $("#" + options.useElement).html() : content;
        $(this).attr("title", "");
        if (content != "" && content != undefined){
          $("body").append("<div id='"+ options.tooltipId + "'>" + content + "</div>");
          $("#" + options.tooltipId)
            .css("position", "absolute")
            .css("top", (e.pageY - options.yOffset) + "px")
            .css("left", (e.pageX + options.xOffset) + "px")
            .css("display", "none")
            .fadeIn("fast")
        }
      });

      $(this).on("mouseleave.easyTooltip", function() {
        $("#" + options.tooltipId).remove();
        $(this).attr("title", title);
      });

      $(this).on("mousemove.easyTooltip", function(e) {
        $("#" + options.tooltipId)
          .css("top",(e.pageY - options.yOffset) + "px")
          .css("left",(e.pageX + options.xOffset) + "px")
      });

      if(options.clickRemove) {
        $(this).on("mousedown.easyTooltip", function(e) {
          $("#" + options.tooltipId).remove();
          $(this).attr("title",title);
        });
      }
    });
  };

})(jQuery);
