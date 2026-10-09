FROM mcr.microsoft.com/devcontainers/cpp:noble

RUN apt-get update && apt-get install -y \
    pkg-config \
    cmake \
    ninja-build \
    libx11-dev \
    libxext-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxfixes-dev \
    libxi-dev \
    libxss-dev \
    libxtst-dev \
    libxkbcommon-dev \
    libdrm-dev \
    libgbm-dev \
    libgl1-mesa-dev \
    libgles2-mesa-dev \
    libegl1-mesa-dev \
    libdbus-1-dev \
    libudev-dev \
    && rm -rf /var/lib/apt/lists/