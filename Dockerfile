FROM ubuntu:24.04 AS builder

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    curl \
    git \
    build-essential \
    cmake \
    clang \
    libssl-dev \
    ca-certificates \
    python3-pip \
    python3-setuptools \
    python3-wheel \
    pipx\
    && rm -rf /var/lib/apt/lists/*

RUN pipx ensurepath

ENV PATH="${PATH}:/root/.local/bin"

RUN pipx install conan

WORKDIR /app

COPY . .

RUN conan profile detect --force

RUN conan install . --build=missing -s build_type=Release

RUN cmake . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake" \
    -G "Unix Makefiles" \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=/app/build/Release

RUN make -j$(nproc) || (echo "Make failed" && exit 1)
RUN ls -l /app/build/Release/

FROM ubuntu:24.04

# RUN apt-get update && \
#     apt-get install -y --no-install-recommends \
#     libssl1.1 \
#     && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/build/Release/logger_service /usr/local/bin/logger_service

COPY config.json /app/config.json

WORKDIR /app

CMD ["logger_service"]