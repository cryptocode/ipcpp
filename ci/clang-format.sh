#!/bin/bash

REPO_ROOT=$(git rev-parse --show-toplevel)
find ${REPO_ROOT}/src -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i
find ${REPO_ROOT}/include -iname '*.hpp' | xargs clang-format -i
