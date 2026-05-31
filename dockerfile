FROM alpine:latest

RUN apk update && apk upgrade
RUN apk add make vim clang tor curl
# ENV ALL_PROXIES="localhost:9050"

WORKDIR /ping_app

ENTRYPOINT ["sleep", "infinity"]