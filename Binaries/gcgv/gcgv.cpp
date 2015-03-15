// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "GCGVBrowser_App.h"
#include "CefBaseApp.h"
#include <GCGV_Callbacks.h>

// Entry point function for all processes.
int main(int argc, char* argv[]) {
  
  // SimpleApp implements application-level callbacks. It will create the first
  // browser instance in OnContextInitialized() after CEF has initialized.
  CefRefPtr<CefBaseApp> app(new CefBaseApp);


  app->Browser().InstallHandler(
      std::shared_ptr<CefBrowserProcessHandler>(
          new GCGVBrowser_App(*app.get())));

  GCGV_Callbacks::InstallNewHandlers(app->Client());

  return app->Main(argc,argv);

}
