#!/bin/sh
# Writes the single-file forms that are minted on Compiler Explorer: layout.hpp is pasted in place of its #include.
# Usage: ./make_single_files.sh   (writes demo_single.cpp, fail_budget_single.cpp, fail_desig_single.cpp next to the sources)
cd "$(dirname "$0")" || exit 2
for n in demo fail_budget fail_desig; do
  awk '/#include "layout.hpp"/{while((getline l < "layout.hpp")>0) if(l !~ /^#pragma once/) print l; next}1' "$n.cpp" > "${n}_single.cpp"
done
