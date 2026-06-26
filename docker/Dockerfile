FROM ubuntu:26.04 AS build

ARG BUILD_TAG

RUN apt update
RUN apt install -y gcc g++ cmake git uuid-dev

WORKDIR /tmp

RUN git clone https://github.com/LazyPanda07/DiscordClone -b ${BUILD_TAG}

WORKDIR /tmp/DiscordClone

RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=/opt/DiscordClone -DBUILD_CLIENT=OFF -DBUILD_CLIENT_LIBRARY=OFF .. && cmake --build . -j && cmake --install .

FROM ubuntu:26.04 AS deploy

ENV LISTEN_PORT=8080
ENV LD_LIBRARY_PATH=/opt/DiscordClone

RUN apt update && apt upgrade -y

COPY --from=build /opt/DiscordClone/bin /opt/DiscordClone

WORKDIR /opt/DiscordClone
ENTRYPOINT [ "sh", "-c", "./DiscordCloneServer --ip 0.0.0.0 --port ${LISTEN_PORT}" ]
