#!/usr/bin/env bash -e
PLISTBUDDY=/usr/libexec/plistbuddy
TARGET_FILE=$1

[ -e "$1" ] || (echo "Could not find file: \"$1\"" && exit -1)

$PLISTBUDDY "$TARGET_FILE" -c "add :BuildMachineOSBuild string" 2> /dev/null || true
$PLISTBUDDY "$TARGET_FILE" -c "set :BuildMachineOSBuild `system_profiler SPSoftwareDataType | grep "System Version" | sed -e "s/^.*(\([0-9A-Z]\{1,\}\)).*$/\1/"`"

$PLISTBUDDY "$TARGET_FILE" -c "add :CFBundleExecutable string" 2> /dev/null || true
$PLISTBUDDY "$TARGET_FILE" -c "set :CFBundleExecutable GameOfLife"

$PLISTBUDDY "$TARGET_FILE" -c "add :CFBundleExecutable string" 2> /dev/null || true
$PLISTBUDDY "$TARGET_FILE" -c "set :CFBundleExecutable GameOfLife"

$PLISTBUDDY "$TARGET_FILE" -c "delete :CFBundleIconFile" 2> /dev/null || true

$PLISTBUDDY "$TARGET_FILE" -c "set :CFBundleIdentifier $($PLISTBUDDY "$TARGET_FILE" -c "print :CFBundleIdentifier" | sed -e "s/\${PRODUCT_NAME.*/GameOfLife/")"

$PLISTBUDDY "$TARGET_FILE" -c "add :CFBundleName" 2> /dev/null || true
$PLISTBUDDY "$TARGET_FILE" -c "set :CFBundleName GameOfLife"

# TODO:
#    <key>CFBundleSupportedPlatforms</key>
#    <array>
#        <string>MacOSX</string>
#    </array>
#    <key>DTCompiler</key>
#    <string>com.apple.compilers.llvm.clang.1_0</string>
#    <key>DTPlatformBuild</key>
#    <string>7D1014</string>
#    <key>DTPlatformVersion</key>
#    <string>GM</string>
#    <key>DTSDKBuild</key>
#    <string>15E60</string>
#    <key>DTSDKName</key>
#    <string>macosx10.11</string>
