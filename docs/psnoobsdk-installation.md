# PSNoobSDK installation help

## Linux

- Install cmake, `build-essential` and `ninja-build`
- Download the latest release and extract the zip into `/opt/psn00bsdk`
- Add the new `/opt/psn00bsdk/bin` dir to your `PATH`
- Set `PSN00BSDK_LIBS="/opt/psn00bsdk/lib/libpsn00b"`
- Re-source your env vars (e.g. `source ~/.profile`) and test with `which mkpsxiso`, `echo $PSN00BSDK_LIBS`

You now have the `mipsel-none-elf-gcc` compiler ready to go

### Creating a new project

Use a template to start a new project

```shell
cp /opt/psn00bsdk/template .
```

Scaffold the build directory (this will also configure some CMake files)

```
cmake -S . -B ./build -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=/opt/psn00bsdk/lib/libpsn00b/cmake/sdk.cmake
```

## Windows

- Set up cmake for windows; MSys2 seems to be a common solution for this
- Download `gcc-mipsel-none-elf-12.3.0-windows.zip` - this is the MIPS toolchain. Extract and add its `/bin` folder to
  the windows PATH. I suggest `C:\PSn00bSDK`
- Download `PSn00bSDK-0.24-win32.zip`, add its bin folder to the PATH too
- Download the latest binaries and extract the zip
- Check if `mkpsxiso` (from SDK) and `mipsel-none-elf-size` (from MIPS toolchain) are recognised on CMD
- Set env var `PSN00BSDK_LIBS` to the libs folder e.g. `C:/PSn00bSDK/lib/libpsn00b`. Check you can echo `echo %PSN00BSDK_LIBS%` and if not restart shell
- now can invoke compiler with `mipsel-none-elf-gcc`

Then call

To rebuild, clear build dir and re-run commands above

### IDE integration

For VSCode, configure the `includePath` in the C/C++ plugin to include `C:\PSn00bSDK/include/libpsn00b`

## Getting a template project

Looks at the templates in `/opt/psn00bsdk` (Linux) or `C:PSn00bSDK/share` (Windows)

## Running a build

In your source folder

```
cmake --preset default .
cmake --build ./build
```
