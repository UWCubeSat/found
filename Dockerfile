FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

# Consolidate APT actions into one layer
RUN apt-get update && apt-get -y upgrade && \
    apt-get install -y \
        git \
        g++ \
        cmake \
        wget \
        tar \
        valgrind \
        python3 \
        python3-pip \
        doxygen \
        graphviz && \
    rm -rf /var/lib/apt/lists/*

# Install Python tools (in one layer)
RUN pip install --break-system-packages \
    git+https://github.com/cpplint/cpplint.git@2.0.0#egg=cpplint \
    git+https://github.com/gcovr/gcovr.git@8.3#egg=gcovr


