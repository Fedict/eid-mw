FROM debian:unstable
COPY experimental.list /etc/apt/sources.list.d/
RUN apt-get update && apt-get -y install eatmydata && eatmydata apt-get -y install --no-install-recommends autoconf automake autoconf-archive gettext libtool libpcsclite-dev libp11-kit-dev libassuan-dev libssl-dev libxml2-dev libcurl4-openssl-dev libproxy-dev plantuml zip gettext build-essential g++ git graphviz libbsd-dev && eatmydata apt-get -y -t experimental install libxml2-dev libgtk-4-dev libgtk-3-dev- && apt-get clean
