$BUILD = "build"

if (Test-Path $BUILD) {
    Remove-Item $BUILD -Recurse
}

cmake --preset default .
cmake --build ./build
