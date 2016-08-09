#!/usr/bin/env bash -e
TUP_PLATFORM=$1

[ $TUP_PLATFORM ] || (echo "TUP_PLATFORM was not set")

shift

for var in "$@"
do
  if ([ "$var" != "ResourcePath.mm" ] && [ "$var" != "ResourcePath.cpp" ]) || ([ "$var" == "ResourcePath.mm" ] && [ "$TUP_PLATFORM" == "macosx" ]) || ([ "$var" == "ResourcePath.cpp" ] && [ "$TUP_PLATFORM" != "macosx" ])
  then
    echo ": $var |> !cxx |> \$(INTERMEDIATE_DIR)/%B.o ../<objs>"
  fi
done
