if [[ $_ == $0 ]]; then  
  echo "This script is meant to be sourced:"
  echo "  source $0"
  exit 0
fi

#
function pathadd() {
  # TODO add check for empty path
  # and what happens if $1 == $2
  # Copy into temp variables
  PATH_NAME=$1
  PATH_VAL=${!1}
  if [[ ":$PATH_VAL:" != *":$2:"* ]]; then
    PATH_VAL="$2${PATH_VAL:+":$PATH_VAL"}"
    echo "- $1 += $2"

    # use eval to reset the target
    eval "$PATH_NAME=$PATH_VAL"
  fi
}

BASE_DIR=$( readlink -f $(dirname $BASH_SOURCE)/ )

pathadd LD_LIBRARY_PATH "${BASE_DIR}/build/herd-library"
pathadd LD_LIBRARY_PATH "${BASE_DIR}/lib"
pathadd LD_LIBRARY_PATH "/opt/BUTool/lib/"

export LD_LIBRARY_PATH

export SWATCH_LOG4CPLUS_CONFIG=${BASE_DIR}/herd-library/log4cplus.properties