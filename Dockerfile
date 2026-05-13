FROM debian:trixie-slim

RUN set -eux; \
    apt-get update; \
    apt-get install -y python3 python3-fastapi sqlite3;

COPY server /app/server
COPY scripts/setup_db.sh /app

WORKDIR /app
RUN ./setup_db.sh

WORKDIR /app/server

EXPOSE 8000

CMD ["python3", "main.py"]
