FROM tianon/sbuild
RUN apt-get update && apt-get -y install eatmydata && eatmydata apt-get -y --no-install-recommends install debhelper devscripts && apt-get clean
COPY setup-chroot.sh /usr/local/bin
