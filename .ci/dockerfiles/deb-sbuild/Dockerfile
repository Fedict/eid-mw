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

# https://github.com/tianon/debian-bin
# useful debian-related (esp. sbuild) scripts
RUN set -ex; \
	wget -O /tmp/debian-bin.tgz 'https://github.com/tianon/debian-bin/archive/91a5cd2a0a57fd72974bcf75e5def3600d2a68ea.tar.gz'; \
	mkdir /tianon-debian-bin; \
	tar -xvf /tmp/debian-bin.tgz -C /tianon-debian-bin --strip-components=1; \
	rm /tmp/debian-bin.tgz
ENV PATH $PATH:/tianon-debian-bin/generic

# usage: download-debuerreotype-tarball.sh suite [arch]
#    ie: download-debuerreotype-tarball.sh stretch
#        download-debuerreotype-tarball.sh sid i386
COPY download-debuerreotype-tarball.sh /tarballs/
RUN ln -s /tarballs/download-debuerreotype-tarball.sh /usr/local/bin/

WORKDIR /tmp

COPY setup-chroot.sh /usr/local/bin
