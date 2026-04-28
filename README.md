![Version](https://img.shields.io/badge/version-0.1.0-orange)
![CMake Version](https://img.shields.io/badge/CMake-3.30+-064F8C?logo=cmake)

# anson.cmake
The cmake peer of [Antson](https://github.com/odys-z/antson). 

# Run Tests (VCPKG)

Anson.cmake reguires dependencies like
[entt](https://github.com/skypjack/entt),
[Boost.URL](https://github.com/boostorg/url) and 
[nlohmann.json](https://github.com/nlohmann/json).

These packages are compiled and tested with Vcpkg, requiring 
install those dependencies on a folder parallel to this folder
on the local machine. See Cmakelists.txt's *VCPKG_INSTALLED_DIR*
definition.

```
    cd ../vcpkg
    ./vcpkg install entt:x64-windows # for Windows
    ./vcpkg install nlohmann-json
    ./vcpkg install boost-url
    ...
```
