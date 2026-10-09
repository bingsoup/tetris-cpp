Package: libxcrypt:x64-linux@4.5.2

**Host Environment**

- Host: x64-linux
- Compiler: GNU 13.3.0
- CMake Version: 4.4.3
-    vcpkg-tool version: 2026-09-26-51bf87ca6e9bf3e622d84ff323bd202ab1ca0c0b
    vcpkg-scripts version: 27504013 2026-10-07 (2 days ago)

**To Reproduce**

`vcpkg install `

**Failure logs**

```
Downloading https://github.com/besser82/libxcrypt/archive/v4.5.2.tar.gz -> besser82-libxcrypt-v4.5.2.tar.gz
Successfully downloaded besser82-libxcrypt-v4.5.2.tar.gz
-- Extracting source /usr/local/vcpkg-downloads/besser82-libxcrypt-v4.5.2.tar.gz
-- Using source at /usr/local/vcpkg/buildtrees/libxcrypt/src/v4.5.2-7ca15a2a8e.clean
-- Getting CMake variables for x64-linux
-- Loading CMake variables from /usr/local/vcpkg/buildtrees/libxcrypt/cmake-get-vars_C_CXX-x64-linux.cmake.log
CMake Error at /workspaces/tetris-cpp/vcpkg_installed/x64-linux/share/vcpkg-make/vcpkg_make.cmake:108 (message):
  libxcrypt currently requires the following programs from the system package
  manager:

      autoconf autoconf-archive automake libtoolize



      On Debian and Ubuntu derivatives:
          sudo apt install autoconf autoconf-archive automake libtool
      On recent Red Hat and Fedora derivatives:
          sudo dnf install autoconf autoconf-archive automake libtool
      On Arch Linux and derivatives:
          sudo pacman -S autoconf autoconf-archive automake libtool
      On Alpine:
          apk add autoconf autoconf-archive automake libtool
      On macOS:
          brew install autoconf autoconf-archive automake libtool

Call Stack (most recent call first):
  /workspaces/tetris-cpp/vcpkg_installed/x64-linux/share/vcpkg-make/vcpkg_make_configure.cmake:66 (vcpkg_run_autoreconf)
  ports/libxcrypt/portfile.cmake:16 (vcpkg_make_configure)
  scripts/ports.cmake:209 (include)



```

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "name": "tetris-cpp",
  "version-string": "0.1.0",
  "dependencies": [
    {
      "name": "sdl3",
      "features": [
        "x11"
      ]
    },
    "sdl3-ttf"
  ]
}

```
</details>
