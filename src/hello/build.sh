#!/bin/env bash
# A dirty script to safely run make (kbuild) without output files laying all over the src
# Errors are expected.

# Shell: enable extended globbing
shopt -s extglob

# Set target paths
SRC="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
OUT=${SRC}/../../out/$(basename ${SRC})
ARG="$1"
# Debugging
# echo "SRC = $SRC"
# echo "OUT = $OUT"
# echo "ARG = $ARG"

# Check for argument
if [[ -z "$ARG" ]]; then ARG="build"; fi

build() {
    # Copy
    [[ -d ${OUT} ]] && rm -rf ${OUT}
    mkdir -p ${OUT}
    cp ${SRC}/* ${OUT}

    # Build
    cd ${OUT}
        make
    cd ${SRC}

    # Clean up:
    # Remove everything that is not *.ko
    for each in *; do rm ${OUT}/${each}; done
    rm ${OUT}/!(*.ko)
    rm ${OUT}/.*
}

clean() {
    [[ -d ${OUT} ]] && rm -rf ${OUT}
}

$(${ARG})

