FROM alpine:latest

RUN apk update && apk upgrade
RUN apk add make vim clang

WORKDIR /ping_app

ENTRYPOINT ["sleep", "infinity"]