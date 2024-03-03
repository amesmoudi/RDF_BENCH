FROM adpart_base_image:latest
RUN dbus-uuidgen --ensure \
    && git clone https://github.com/razen-alharbi/AdPart.git \
    && cd /root/AdPart \
    && qmake AdHashGUI.pro -r -spec linux-g++ \
    && make -j4
RUN mkdir -p /var/run/sshd
ENV ADHASH_HOME=/root/AdPart
WORKDIR /root/AdPart

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]