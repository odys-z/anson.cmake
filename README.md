![Version](https://img.shields.io/badge/version-0.1.0-orange)
![CMake Version](https://img.shields.io/badge/CMake-3.30+-064F8C?logo=cmake)

# anson.cmake
The cmake peer of [Antson](https://github.com/odys-z/antson). 

# Run Tests (VCPKG)

## Prerequisit

- Jun 4, 2026

Switch to MinGW with g++ 16.1.0.

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

# ASTs for Tests

Use the latest Semantier-generator for generating required headers for testing.

```
    pip install semantier-generator
    python -m semantier_gen settings/....json ast # TODO docs
```

AST files, *.ast.json, are copied from Anson.cmake/tests/ast.

# Compile on Ubuntu

## Prerequisites

Install build tooling:

```
    sudo apt update
    sudo apt install -y build-essential ninja-build git curl zip unzip tar \
        pkg-config autoconf automake libtool python3
```

CMake must be v4.4.2 or newer. Ubuntu's `apt` package, and the `cmake` PyPI wheel,
are both normally too old to satisfy this. Download a current build from the
official [CMake releases page](https://cmake.org/download/) instead:

```
    cd /tmp
    wget https://github.com/Kitware/CMake/releases/download/v4.4.2/cmake-4.4.2-linux-x86_64.tar.gz
    tar xzf cmake-4.4.2-linux-x86_64.tar.gz
    sudo mv cmake-4.4.2-linux-x86_64 /opt/cmake-4.4.2
    sudo ln -sf /opt/cmake-4.4.2/bin/cmake /usr/local/bin/cmake
```
### Compiler version

Requires `g++-16`.

FYI, installing a newer one does not automatically make it the default — `g++`/`gcc`
are usually symlinks managed by `update-alternatives`. To make a newer version
the default:

```
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-16 16
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-16 16
    sudo update-alternatives --config g++
    sudo update-alternatives --config gcc
```

## vcpkg

Clone vcpkg as a sibling directory to this project (`../vcpkg` relative to this repo)
and bootstrap it:

```
    cd ..
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
```

Install the dependencies for the Linux triplet:

```
    ./vcpkg install boost-url:x64-linux openssl:x64-linux \
        nlohmann-json:x64-linux entt:x64-linux
```

`CMakeLists.txt` auto-selects the `x64-linux` triplet on Linux, so no extra
`-DVCPKG_TARGET_TRIPLET` flag is needed for a native build. Pass one explicitly
only if you want to cross-compile (e.g. Windows binaries via MinGW).

## Build

```
    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
    cmake --build build
```

GoogleTest is fetched automatically via `FetchContent` when tests are enabled
(`ANSON_BUILD_TESTS`, on by default), so make sure git/network access (and any
proxy configuration) is available at configure time.
