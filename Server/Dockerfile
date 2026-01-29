FROM gcc:latest

# Installiamo le librerie necessarie (MySQL + Nano + cJSON)
RUN apt-get update && \
    apt-get install -y default-libmysqlclient-dev nano libcjson-dev

WORKDIR /usr/src/app

# Non copiamo il codice qui, lo monteremo come volume