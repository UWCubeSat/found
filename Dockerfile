FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive \
    MAMBA_ROOT_PREFIX=/opt/micromamba \
    PATH=/opt/micromamba/bin:$PATH \
    MAMBA_DOCKERFILE_ACTIVATE=1

# Install system dependencies and micromamba
RUN apt-get update && apt-get -y upgrade && \
    apt-get install -y --no-install-recommends \
        curl \
        ca-certificates \
        bzip2 && \
    curl -L https://micromamba.snakepit.net/api/micromamba/linux-64/latest | \
        tar -xvj -C /usr/local/bin --strip-components=1 bin/micromamba

# 🟢 Initialize micromamba for all future shells (important)
RUN echo 'eval "$(micromamba shell hook --shell bash)"' >> /etc/bash.bashrc

# Clean up micromamba install tools and continue with other packages
RUN apt-get purge -y curl bzip2 && \
    apt-get autoremove -y && \
    apt-get clean
    
# Copy the install script into the container
COPY install.sh /tmp/install.sh

# Run the script and remove it in a single layer
RUN chmod +x /tmp/install.sh && /tmp/install.sh || (cat /tmp/install.log && exit 1)







