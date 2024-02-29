FROM adpart_base_image:latest
COPY scripts/startup.sh /root/
RUN dbus-uuidgen --ensure \
    && git clone https://github.com/razen-alharbi/AdPart.git \
    && cd /root/AdPart \
    && qmake AdHashGUI.pro -r -spec linux-g++ \
    && make -j4

ENV ADHASH_HOME=/root/AdPart
WORKDIR /root/AdPart
CMD ["sh", "/root/startup.sh"]