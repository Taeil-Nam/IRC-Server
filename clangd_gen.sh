#!/bin/bash

pwd=$(pwd)

echo "CompileFlags:" > .clangd
echo "  Add: [-I$pwd/src, -I$pwd/extlibs/libbsd-gdf/include, -std=c++98]" >> .clangd