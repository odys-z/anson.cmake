# anson.cmake
The cmake peer of Antson. 

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
