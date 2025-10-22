# The following was (mostly) copied shamelessly from
# https://github.com/tianon/dockerfiles, sbuild/Dockerfile, but with the Debian
# distribution and a few things changed so we get a more recent version of
# debootstrap.

FROM ubuntu:rolling

RUN set -ex; \
	apt-get update; \
	apt-get -y upgrade; \
	apt-get install -y --no-install-recommends \
		sbuild zstd uidmap mmdebstrap arch-test\
		\
		debian-archive-keyring ubuntu-keyring \
		wget ca-certificates \
		\
		apt-utils \
		devscripts gpgv libwww-perl file \
		fakeroot \
		rsync sudo unzip eatmydata debhelper devscripts \
	; \
	rm -rf /var/lib/apt/lists/*

RUN echo 'root:100000:65536' > /etc/subuid; echo 'root:100000:65536' > /etc/subgid

WORKDIR /tmp
