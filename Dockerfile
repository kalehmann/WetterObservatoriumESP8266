FROM debian:12-slim

RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install --no-install-recommends -y \
    build-essential \
    ca-certificates \
    git \
    python3 \
    && rm -rf /var/lib/apt/lists/*
