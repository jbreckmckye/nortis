# PSX build

Call `build.sh` (*nix) or `build.ps1` (Windows). Requires Libpsnoob installed (instructions below)

For copyright reasons I have gitignored the `license_data.dat`. If you want to build without one, comment out the 
following line in `iso.xml`:

```xml
<license file="${PROJECT_SOURCE_DIR}/license_data.dat" />
```

Otherwise, you will need to extract a license file from a retail PSX game, e.g. using `dumpsxiso`

### Installing PSNoobSDK (Windows)

Requires cmake https://cmake.org/download (I used 3.27.8)
Rest of instructions from https://github.com/Lameguy64/PSn00bSDK
- download binaries for platform, releases here: https://github.com/Lameguy64/PSn00bSDK/releases - I used v0.24
    - downloaded gcc-mipsel-none-elf-12.3.0-windows.zip - this is the MIPS toolchain. Added its /bin to my PATH
    - same dealy with PSn00bSDK-0.24-win32.zip
    - can check if mkpsxiso (from SDK) and mipsel-none-elf-size (from MIPS toolchain) are recognised on CMD
- set env var `PSN00BSDK_LIBS` to the libs folder e.g. C:/PSn00bSDK/lib/libpsn00b. Check you can echo `echo %PSN00BSDK_LIBS%` and if not restart shell
- now can invoke compiler with `mipsel-none-elf-gcc`

Then can create project as per https://github.com/Lameguy64/PSn00bSDK/blob/master/doc/installation.md#creating-a-project
Copy template from share\psn00bSDK\template into a new repo

To build a project, call
```
cmake --preset default .
cmake --build ./build
```

To rebuild, clear build dir and re-run commands above

For VSCode, configure the `includePath` in the C/C++ plugin to include `C:\PSn00bSDK/include/libpsn00b` - this enables intellisense for PSX headers
