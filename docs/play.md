# Build and play

## PlayStation

- You'll need to install PSNoobSDK, CMake, and a MIPS C buildchain for your system. There are instructions on the PSNoobSDK
  repo, but for version 0.24 (the latest) you can use my [psnoobsdk installation instructions](./psnoobsdk-installation.md).
- In the `psx` folder, run `cmake -S . -B ./build -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=/opt/psn00bsdk/lib/libpsn00b/cmake/sdk.cmake`.
  This is required to fully configure the CMake build.
- By default, the `iso.xml` refers to a `<license>`, which is what shows the PSX logo. This has to be `gitignored` for
  copyright reasons. If you want one, you can get a `license_data.dat` from a 'real' PSX game using `dumpsxiso`, which
  comes with psnoobsdk. Alternatively just comment out this line.
- Use `build.ps1` (windows + powershell) or `build.sh` (Linux)
- This should generate a bin/cue in `build` which you can run in an emulator or burn to CD-ROM

## MacOS

There is also a MacOS build from when I was learning C and used SDL as my IO/graphics framework. You want to run the 
`build-macos` task in the `package.json` using NPM / Yarn

## Web

There was also a quick prototype written in JavaScript with canvas. Run `dev-web` to serve this on localhost. You'll need
to run `yarn install`.
