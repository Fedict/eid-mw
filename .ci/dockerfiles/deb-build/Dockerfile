ARG CI_DEPENDENCY_PROXY_GROUP_IMAGE_PREFIX=docker.io
FROM $CI_DEPENDENCY_PROXY_GROUP_IMAGE_PREFIX/debian:stable
RUN apt-get update && apt-get -y install eatmydata && eatmydata apt-get -y install --no-install-recommends autoconf-archive autoconf automake libtool libpcsclite-dev libp11-kit-dev libgtk-3-dev libassuan-dev libgpg-error-dev libssl-dev libxml2-dev libcurl4-openssl-dev libproxy-dev plantuml zip gettext build-essential g++ git graphviz && apt-get clean
