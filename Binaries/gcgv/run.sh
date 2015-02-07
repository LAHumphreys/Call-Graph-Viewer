#!/bin/bash 

if [[ "$CEF_DIR" == "" ]]; then
    echo "Error, CEF_DIR must be set to the location of the CEF resources!"
    exit 1
fi

make chrome-sandbox || exit 1
make .LINK_RESOURCES || exit 1

./viewer --url=file://$PWD/Content/StartPage.html
