# Use Debian as the base image
FROM debian:bullseye-slim

# Set environment variables to minimize interaction during build
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages and dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ \
    cmake \
    make \
    curl \
    git \
    libcurl4-openssl-dev \
    libssl-dev \
    libpthread-stubs0-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set working directory for temporary files
WORKDIR /tmp

# Install CPR library
RUN git clone https://github.com/libcpr/cpr.git \
    && cd cpr \
    && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCPR_ENABLE_SSL=ON . \
    && make -j$(nproc) \
    && make install \
    && rm -rf /tmp/cpr

# Install Crow library
RUN git clone https://github.com/CrowCpp/crow.git \
    && cp -r crow/include/* /usr/local/include/ \
    && rm -rf /tmp/crow

# Set the application working directory
WORKDIR /app

# Copy source code into the container
COPY . /app

# Compile the application
RUN g++ main.cpp src/OrderController.cpp src/OrderService.cpp -o app \
    -L/usr/local/lib -lcpr -lpthread -I/usr/local/include

# Expose a port if your app listens on one (optional, replace 8080 with your app's port)
EXPOSE 8080

# Run the application by default
CMD ["./app"]
