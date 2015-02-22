Graphical CGV Libraries
=======================

Overview
---------
GCGV uses the Chromium Embeded Framework (CEF) to build a html / javascript based front end for the CGV libraries. 

It is assumed the reader is familiar with the basic structure of a CEF application, and is aware of the CEF [general usage guide](https://code.google.com/p/chromiumembedded/wiki/GeneralUsage).

The GCGV layer comprises several libraries, in linking order (with the highest level first) these are:

Library | Purpose
--------- | -----------
libGCGVApp | Implements CefApp, which handles application level events
libGCGVCallbacks | Implements CefClient, which defines handlers for Cef Events
libGCGVJS | Defines java-script functions, and handles inter-processing message routing for java script callbacks

libGCGVApp
---------

CefApp is passed to CEF during initialisation, and defines our application. It has handlers relating to the creation of new browser / renderer processes. In addition to doing any initialisation it is responsible for constructing a new instance of CefClient (in our case implemented by libGCGVCallbacks), which will define the behaviour for this process,  and passing it to these proceses.

libGCGVCallbacks
-----

CefClient handles events within existing Browser / Renderer processes. These include
- Display changes (such as a window title) which require the underlying display library (GTK) to be notified
- Page navigation (the browser process is notified at the start / end of a frame load)
- Creation / Destruction of new browsers (when the last browser closes we must terminate the application)

libGCGVJS
-----
Defines extensions to the java-script interface. This library is responsible for
- Defining new functions available in js
- Handling async requests from the render and routing them to the browser process
