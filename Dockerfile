FROM ubuntu:24.04
RUN apt-get update && apt-get install -y gcc make libsqlite3-dev && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY . .
RUN make
RUN mkdir -p data
EXPOSE 8080
CMD ["./build/aerotask-server","8080"]
