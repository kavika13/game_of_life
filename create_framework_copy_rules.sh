#!/usr/bin/env bash -e

[ $1 ] || (echo "base path was not set" && exit -1)
[ $2 ] || (echo "output path was not set" && exit -1)
[ $3 ] || (echo "framework name was not set" && exit -1)
[ $4 ] || (echo "framework version was not set" && exit -1)

echo ": $1/$3.framework/Versions/$4/$3 |> !cp |> $2/$3.framework/Versions/$4/$3"
echo ": $1/$3.framework/Versions/$4/Resources/Info.plist |> !cp |> $2/$3.framework/Versions/$4/Resources/Info.plist"
echo ": |> ln -s $4 \"%o\" |> $2/$3.framework/Versions/Current"
echo ": |> ln -s \"Versions/Current/$3\" \"%o\" |> $2/$3.framework/$3"
echo ": |> ln -s \"Versions/Current/Resources\" \"%o\" |> $2/$3.framework/Resources"
