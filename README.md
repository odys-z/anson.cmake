![Version](https://img.shields.io/badge/version-0.1.0-orange)
![CMake Version](https://img.shields.io/badge/CMake-3.30+-064F8C?logo=cmake)

# anson.cmake
The cmake peer of [Antson](https://github.com/odys-z/antson). 

# Run Tests (VCPKG)

## Prerequisit

- Jun 4, 2026

Using Qt MinGW with g++ 16.1.0.

Note the official Qt Online Installer does not ship a MinGW package higher than GCC 13.1.
This source use distribution from [x86_64-16.1.0-release-posix-seh-ucrt-rt_v14-rev1.7z](https://github.com/niXman/mingw-builds-binaries/releases).

Anson.cmake reguires dependencies like
[entt](https://github.com/skypjack/entt),
[Boost.URL](https://github.com/boostorg/url) and 
[nlohmann.json](https://github.com/nlohmann/json).

These packages are compiled and tested with Vcpkg, requiring 
install those dependencies on a folder parallel to this folder
on the local machine. See Cmakelists.txt's *VCPKG_INSTALLED_DIR*
definition.

Add to vcpkg/triplets/community/x64-mingw-dynamic.cmake

```
    set(ENV{CC} "path-to/mingw64-gcc16.1.0/bin/gcc.exe")
    set(ENV{CXX} "path-to/mingw64-gcc16.1.0/bin/g++.exe")
    set(ENV{RC} "path-to/mingw64-gcc16.1.0/bin/windres.exe")
```

Install denpendencies

```
    cd ../vcpkg
    ./vcpkg install boost-url:x64-mingw-dynamic openssl:x64-mingw-dynamic nlohmann-json:x64-mingw-dynamic
    ./vcpkg install entt:x64-mingw-dynamic 
```
