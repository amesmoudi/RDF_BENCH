FROM gstored-base:latest
RUN dbus-uuidgen --ensure \
    && git clone https://github.com/bnu05pp/gStoreD.git \
    && git clone https://github.com/bnu05pp/mpc.git \
    && cd /root/mpc \
    && g++ main.cpp -std=c++11 -o mpc \
    && cd /root/gStoreD \
    && make 
RUN mkdir -p /var/run/sshd
WORKDIR /root/gStoreD
EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
