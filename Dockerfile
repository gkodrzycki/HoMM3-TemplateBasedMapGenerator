FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    nodejs \
    npm \
    liblua5.4-0 \
    liblua5.4-dev \
    libmagicenum-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m appuser

WORKDIR /app

COPY website/backend/package*.json ./website/backend/
RUN cd website/backend && npm install

COPY . .
RUN make

RUN chown -R appuser:appuser /app

USER appuser

EXPOSE 3000

CMD ["node", "website/backend/server.js"]
