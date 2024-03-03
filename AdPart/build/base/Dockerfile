FROM ubuntu:trusty

RUN apt-get update \
    && apt-get install -y nano gcc g++ build-essential mpich2 libmpich2-dev libboost-dev libboost-iostreams-dev libboost-system-dev libboost-filesystem-dev libglu1-mesa-dev gnuplot git openssh-server x11-apps xauth qt5-default dbus \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*


# Activation du forwarding X11
RUN echo "X11Forwarding yes" >> /etc/ssh/sshd_config
RUN echo "X11UseLocalhost no" >> /etc/ssh/sshd_config


RUN mkdir -p /root/.ssh/
ADD ssh/* /root/.ssh/
RUN chmod 600 /root/.ssh/id_rsa
RUN chmod 600 /root/.ssh/id_ed25519

WORKDIR /root/