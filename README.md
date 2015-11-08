Call-Graph-Viewer [![Build Status](https://travis-ci.org/Grauniad/Call-Graph-Viewer.png)](https://travis-ci.org/Grauniad/Call-Graph-Viewer) [![Coverage Status](https://coveralls.io/repos/Grauniad/Call-Graph-Viewer/badge.svg?branch=master&service=github)](https://coveralls.io/github/Grauniad/Call-Graph-Viewer?branch=master)
==================
!["Expample screen shots"](https://raw.githubusercontent.com/Grauniad/Call-Graph-Viewer/master/ExampleScreenshot.png "Example screen shot showing the initial CEF GUI screens")


A simple tool for profiling processes using callgraph data.

It already provides:
* A flat view of your most expensive functions
* A complete calltree, including costs / callcount at each node
* regex powered searching of both the call tree and the flat view 
* Code annotation
* Callgrind integration:
  * Easily profile C / C++ and similar languages 
  * (Unlike some viewers), The full call tree (using --separate-callers with callgrind)
* Simple CSV based input allows manual profile data to be used (e.g see the TCL pluging provided)
* Configure your own commands in the rc file

