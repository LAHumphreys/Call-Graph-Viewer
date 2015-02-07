/*global $, document, alert */

"use strict";

/*
 * Prepare hooks for the analyis drop down
 */
function SetupAnalysisModeMenu() {
    SetAnaysisMode("Total Instructions");
    
    $(".analysis-selctor li a").click(function () {
        SetAnaysisMode($(this).text());
    });
}
    
/*
 * Prepare the page:
 *  - Setup the analysis menu so it can flip modes
 */
$(document).ready(function () {
    SetupAnalysisModeMenu();
});