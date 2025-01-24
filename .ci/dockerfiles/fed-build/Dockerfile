# mock on Fedora 40 seems broken, so stick with 39 for now
FROM fedora:39
RUN dnf -y --enablerepo='updates-testing' install mock git rpm-build rpmdevtools && dnf clean all
