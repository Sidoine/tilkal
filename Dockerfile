FROM perl:latest
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN mkdir Release
RUN mkdir Debug
RUN perl proja.pl

FROM gcc:latest
COPY --from=0 /usr/src/myapp /usr/src/myapp
WORKDIR /usr/src/myapp
RUN make release
CMD ["./tilkal_server"]
