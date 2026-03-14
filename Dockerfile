FROM debian:bookworm-slim AS build

RUN apt-get update \
	&& apt-get install -y --no-install-recommends build-essential cmake \
	&& rm -rf /var/lib/apt/lists/*

COPY . /usr/src/myapp
WORKDIR /usr/src/myapp

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
	&& cmake --build build --config Release

FROM debian:bookworm-slim

COPY --from=build /usr/src/myapp/build/tilkal_server /usr/local/bin/tilkal_server
COPY --from=build /usr/src/myapp/text /usr/src/myapp/text
COPY --from=build /usr/src/myapp/www /usr/src/myapp/www
COPY --from=build /usr/src/myapp/images /usr/src/myapp/images

WORKDIR /usr/src/myapp
CMD ["/usr/local/bin/tilkal_server"]
