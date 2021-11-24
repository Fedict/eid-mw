FROM opensuse/leap
RUN zypper install -y git make perl-libwww-perl perl-XML-Parser perl-LWP-Protocol-https && git clone https://github.com/openSUSE/obs-build && cd obs-build && make install && cd .. && rm -rf obs-build
COPY sl15.3.conf /usr/lib/build/configs
