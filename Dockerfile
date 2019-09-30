FROM ubuntu:trusty
EXPOSE 8125 

RUN mkdir -p /statsite && mkdir -p /var/run/statsite && \
    apt-get update && \
    apt-get install -y build-essential check scons libjansson-dev libcurl4-openssl-dev libcurl3 libjansson4 && \
    apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*

ADD . /statsite

RUN (cd statsite && make)

VOLUME ["/etc/statsite"]
CMD ["/statsite/statsite", "-f", "/etc/statsite/statsite.conf"]

