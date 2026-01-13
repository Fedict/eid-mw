ARG CI_DEPENDENCY_PROXY_IMAGE_PREFIX
FROM $CI_DEPENDENCY_PROXY_IMAGE_PREFIX/debian:stable
RUN apt-get update && apt-get -y install eatmydata && eatmydata apt-get -y --no-install-recommends install clang-tools autoconf automake libtool libassuan-dev libgpg-error-dev libpcsclite-dev libp11-kit-dev libgtk-3-dev libssl-dev libxml2-dev libcurl4-openssl-dev libproxy-dev plantuml zip gettext build-essential git g++ graphviz curl && apt-get clean
