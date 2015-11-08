#!/bin/bash

cd DEV_TOOLS
git submodule init
git submodule update
cd EXTERNAL_INCLUDES/websocketpp/
git checkout 0.6.0
git status
