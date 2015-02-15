/*global $, document, extend, Application, Analyse, Analyse_View, Analyse_Presenter */

/*
 * Javascript driving the analyse page
 */

var Analyse_App = {
};

// Load the view components
extend(Application.view, Analyse_View);
extend(Application.model, Analyse);
extend(Application.presenter, Analyse_Presenter);
