#!/usr/bin/env bash -e
PLISTBUDDY=/usr/libexec/plistbuddy
SOURCE_FILE=$1
TARGET_FILE=$2

[ -e "$1" ] || (echo "Could not find input file: \"$1\"" && exit -1)

$PLISTBUDDY "$SOURCE_FILE" -c "print :CFBundlePackageType" | tr -d '\n' >> "$TARGET_FILE"
$PLISTBUDDY "$SOURCE_FILE" -c "print :CFBundleSignature" | tr -d '\n' >> "$TARGET_FILE"
