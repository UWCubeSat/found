FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive \
    MAMBA_ROOT_PREFIX=/opt/micromamba \
    PATH=/opt/micromamba/bin:$PATH \
    MAMBA_DOCKERFILE_ACTIVATE=1

# Install system dependencies and micromamba
RUN apt-get update && apt-get -y upgrade && \
    apt-get install -y --no-install-recommends \
        git \
        g++ \
        cmake \
        wget \
        tar \
        valgrind \
        python3 \
        python3-pip \
        doxygen \
        graphviz \
        curl \
        ca-certificates \
        bzip2 && \
    curl -L https://micromamba.snakepit.net/api/micromamba/linux-64/latest | \
        tar -xvj -C /usr/local/bin --strip-components=1 bin/micromamba && \
    apt-get purge -y curl bzip2 && \
    apt-get autoremove -y && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install Python tools
RUN pip install --break-system-packages \
    git+https://github.com/cpplint/cpplint.git@2.0.0#egg=cpplint \
    git+https://github.com/gcovr/gcovr.git@8.3#egg=gcovr



