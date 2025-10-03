FROM gcc:13

RUN apt-get update && apt-get install -y \
    libgtest-dev \
    cmake \
    && cd /usr/src/googletest \
    && cmake . \
    && make \
    && make install \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN make all

CMD ["./bin/exchange"]