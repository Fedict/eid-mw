ARG CI_DEPENDENCY_PROXY_IMAGE_PREFIX
FROM $CI_DEPENDENCY_PROXY_IMAGE_PREFIX/fedora:latest
RUN dnf -y --enablerepo='updates-testing' install mock git rpm-build rpmdevtools && dnf clean all
