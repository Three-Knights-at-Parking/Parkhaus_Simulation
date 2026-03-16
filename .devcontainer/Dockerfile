FROM mcr.microsoft.com/devcontainers/base:ubuntu

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    doxygen \
    libjson-c-dev \
    pkg-config \
    gnuplot \
    && apt-get clean && rm -rf /var/lib/apt/lists/*