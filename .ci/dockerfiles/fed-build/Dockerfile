ARG CI_DEPENDENCY_PROXY_GROUP_IMAGE_PREFIX=docker.io
FROM $CI_DEPENDENCY_PROXY_GROUP_IMAGE_PREFIX/fedora:latest
RUN dnf -y --enablerepo='updates-testing' install mock git rpm-build rpmdevtools && dnf clean all
