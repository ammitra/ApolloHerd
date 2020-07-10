#!/bin/bash

# 1. Look for doxygen command
if [ -n "${DOXYGEN_BIN_DIR}" ]; then
    DOXYGEN_BIN=${DOXYGEN_BIN_DIR}/doxygen
    if [ -x "${DOXYGEN_BIN}" ]; then
        echo "Cannot find doxygen exe under directory '${DOXYGEN_DIR}'. Aborting"
        exit 1
    fi 
else
    command -v doxygen >/dev/null 2>&1 || { echo "Cannot find doxygen command. Aborting." >&2; exit 1; }
    DOXYGEN_BIN="doxygen"
fi
echo "Using doxygen command: ${DOXYGEN_BIN}"
echo "   (version `doxygen --version`)"


# 2. Set environment variables for doxygen
export REPO_DOXY_DIR=$( readlink -f $(dirname $BASH_SOURCE)/ )
export REPO_BASE_DIR=$( readlink -f $(dirname $BASH_SOURCE)/../ )

export DOXYGEN_HERD_BASE_DIR=${REPO_BASE_DIR}
export DOXYGEN_EXCLUDE_PATTERNS=''
export DOXYGEN_OUTPUT=${REPO_DOXY_DIR}
DOXYGEN_STRIP_FROM_INC_PATH=
for PACKAGE_PATH in logger dummy core action; do
    DOXYGEN_STRIP_FROM_INC_PATH+="${REPO_BASE_DIR}/${PACKAGE_PATH}/include "
done
export DOXYGEN_STRIP_FROM_INC_PATH

echo "Input parameters ..."
echo "  DOXYGEN_HERD_BASE_DIR       = ${DOXYGEN_HERD_BASE_DIR}"
echo "  DOXYGEN_EXCLUDE_PATTERNS    = ${DOXYGEN_EXCLUDE_PATTERNS}"
echo "  DOXYGEN_OUTPUT              = ${DOXYGEN_OUTPUT}"
echo "  DOXYGEN_STRIP_FROM_INC_PATH = ${DOXYGEN_STRIP_FROM_INC_PATH}"

echo "Cleaning up target directory, ${DOXYGEN_OUTPUT}"
rm -rf ${DOXYGEN_OUTPUT}/html


# 3. Main command
${DOXYGEN_BIN} ${REPO_DOXY_DIR}/herd.doxy


