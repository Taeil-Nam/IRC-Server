#!/bin/bash

pwd=$(pwd)

echo "CompileFlags:" > .clangd
echo "  Add: [-I$pwd/src, -std=c++98]" >> .clangd