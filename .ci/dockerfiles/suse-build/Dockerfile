ARG CI_DEPENDENCY_PROXY_IMAGE_PREFIX
FROM $CI_DEPENDENCY_PROXY_IMAGE_PREFIX/opensuse/leap:16.0
RUN zypper install -y hostname findutils cpio zstd git make perl-libwww-perl perl-XML-Parser perl-LWP-Protocol-https && git clone https://github.com/openSUSE/obs-build && cd obs-build && make install && cd .. && rm -rf obs-build
COPY sl16.0.conf /usr/lib/build/configs
