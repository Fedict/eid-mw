FROM fedora:latest
RUN dnf -y --enablerepo='updates-testing' install mock git rpm-build rpmdevtools && dnf clean all && sed -i -e 's/enabled=0/enabled=1/' /etc/mock/templates/centos-8.tpl
