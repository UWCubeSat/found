FROM ubuntu:latest

# Set environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive

# Install ca-certificates for HTTPS support in install.sh
RUN apt-get update && \
    apt-get install -y --no-install-recommends ca-certificates && \
    rm -rf /var/lib/apt/lists/*

COPY install.sh /tmp/install.sh

RUN chmod +x /tmp/install.sh && /tmp/install.sh && \
    rm -f /tmp/install.sh
