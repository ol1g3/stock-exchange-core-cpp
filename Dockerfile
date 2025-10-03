FROM gcc:13

RUN apt-get update && apt-get install -y \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN make all

CMD ["./bin/exchange"]