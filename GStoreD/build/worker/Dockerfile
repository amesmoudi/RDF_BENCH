FROM gstored-base:latest
RUN dbus-uuidgen --ensure \
    && git clone https://github.com/bnu05pp/gStoreD.git \
    && cd /root/gStoreD \
    && make 
RUN mkdir -p /var/run/sshd
WORKDIR /root/gStoreD

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]