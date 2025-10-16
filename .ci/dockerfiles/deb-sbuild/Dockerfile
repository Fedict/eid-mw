# The following was (mostly) copied shamelessly from
# https://github.com/tianon/dockerfiles, sbuild/Dockerfile, but with the Debian
# distribution and a few things changed so we get a more recent version of
# debootstrap.

FROM debian:testing-slim

RUN echo "deb http://deb.debian.org/debian experimental main" >> /etc/apt/sources.list

RUN set -ex; \
	apt-get update; \
	apt-get -y upgrade; \
	apt-get install -y --no-install-recommends \
		sbuild schroot zstd \
		\
		debian-archive-keyring ubuntu-archive-keyring \
		wget ca-certificates \
		\
		apt-utils \
		devscripts gpgv libwww-perl file \
		fakeroot \
		rsync sudo unzip eatmydata debhelper devscripts \
	; \
	apt-get -y -t experimental install debootstrap; \
	rm -rf /var/lib/apt/lists/*

RUN echo 'root:100000:65536' > /etc/subuid; echo 'root:100000:65536' > /etc/subgid

WORKDIR /tmp

COPY setup-chroot.sh /usr/local/bin
