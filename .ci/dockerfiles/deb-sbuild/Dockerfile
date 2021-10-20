# The following was (mostly) copied shamelessly from
# https://github.com/tianon/dockerfiles, sbuild/Dockerfile, but with the Debian
# distribution and a few things changed so we get a more recent version of
# debootstrap.

FROM debian:unstable-slim

RUN echo "deb http://deb.debian.org/debian experimental main" >> /etc/apt/sources.list

RUN set -ex; \
	apt-get update; \
	apt-get -y -t experimental install debootstrap; \
	apt-get install -y --no-install-recommends \
		sbuild schroot zstd \
		\
		debian-archive-keyring ubuntu-archive-keyring \
		wget ca-certificates \
		\
		apt-utils \
		devscripts libwww-perl file \
		fakeroot \
		rsync sudo unzip eatmydata debhelper devscripts \
	; \
	rm -rf /var/lib/apt/lists/*

WORKDIR /tmp

COPY setup-chroot.sh /usr/local/bin
