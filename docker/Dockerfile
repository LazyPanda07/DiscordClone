FROM ubuntu:24.04 AS build

ARG BUILD_TAG

RUN apt update
RUN apt install -y gcc g++ cmake git

WORKDIR /tmp

RUN git clone https://github.com/LazyPanda07/DiscordClone -b ${BUILD_TAG}

WORKDIR /tmp/DiscordClone

RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/DiscordClone -DBUILD_CLIENT=OFF .. && cmake --build . -j && cmake --install .

FROM ubuntu:24.04 AS deploy

ENV LISTEN_PORT=8080

RUN apt update && apt upgrade -y

COPY --from=build /opt/DiscordClone/bin/DiscordCloneServer /opt/DiscordClone/DiscordCloneServer

WORKDIR /opt/DiscordClone
ENTRYPOINT [ "sh", "-c", "./DiscordCloneServer ${LISTEN_PORT}" ]
