Package: libsystemd:x64-linux@260.2#1

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
Downloading https://github.com/mesonbuild/meson/archive/1.9.0.tar.gz -> meson-1.9.0.tar.gz
Successfully downloaded meson-1.9.0.tar.gz
-- Applying patch /workspaces/tetris-cpp/vcpkg_installed/x64-linux/share/vcpkg-tool-meson/meson-intl.patch
-- Applying patch /workspaces/tetris-cpp/vcpkg_installed/x64-linux/share/vcpkg-tool-meson/adjust-python-dep.patch
-- Applying patch /workspaces/tetris-cpp/vcpkg_installed/x64-linux/share/vcpkg-tool-meson/adjust-args.patch
-- Applying patch /workspaces/tetris-cpp/vcpkg_installed/x64-linux/share/vcpkg-tool-meson/remove-pkgconfig-specialization.patch
-- Applying patch /workspaces/tetris-cpp/vcpkg_installed/x64-linux/share/vcpkg-tool-meson/meson-56879d5.diff
-- Found Python version '3.12.3 at /usr/bin/python3'
-- Using meson: /usr/local/vcpkg-downloads/tools/meson-1.9.0-633807/meson.py
Downloading https://github.com/systemd/systemd/archive/v260.2.tar.gz -> systemd-systemd-v260.2.tar.gz
Successfully downloaded systemd-systemd-v260.2.tar.gz
-- Extracting source /usr/local/vcpkg-downloads/systemd-systemd-v260.2.tar.gz
-- Applying patch disable-warning-nonnull.patch
-- Applying patch only-libsystemd.patch
-- Applying patch pkgconfig.patch
-- Applying patch fix-2604-build.patch
-- Using source at /usr/local/vcpkg/buildtrees/libsystemd/src/v260.2-1a875e5b45.clean
-- Setting up python virtual environment...
CMake Error at scripts/cmake/vcpkg_execute_required_process.cmake:127 (message):
    Command failed: /usr/bin/python3 -I -m venv --symlinks /usr/local/vcpkg/buildtrees/libsystemd/x64-linux-venv
    Working Directory: /usr/local/vcpkg/buildtrees/libsystemd
    Error code: 1
    See logs for more information:
      /usr/local/vcpkg/buildtrees/libsystemd/venv-setup-x64-linux-out.log

Call Stack (most recent call first):
  /workspaces/tetris-cpp/vcpkg_installed/x64-linux/share/vcpkg-get-python-packages/x_vcpkg_get_python_packages.cmake:94 (vcpkg_execute_required_process)
  ports/libsystemd/portfile.cmake:19 (x_vcpkg_get_python_packages)
  scripts/ports.cmake:209 (include)



```

<details><summary>/usr/local/vcpkg/buildtrees/libsystemd/venv-setup-x64-linux-out.log</summary>

```
The virtual environment was not created successfully because ensurepip is not
available.  On Debian/Ubuntu systems, you need to install the python3-venv
package using the following command.

    apt install python3.12-venv

You may need to use sudo with that command.  After installing the python3-venv
package, recreate your virtual environment.

Failing command: /usr/local/vcpkg/buildtrees/libsystemd/x64-linux-venv/bin/python3
```
</details>

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
