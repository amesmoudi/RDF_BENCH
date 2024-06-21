# Use Ubuntu Trusty as the base image
FROM ubuntu:trusty

# Update and install required packages
RUN apt-get update && \
    apt-get install -y nano gcc g++ build-essential mpich libmpich-dev libboost-dev libboost-iostreams-dev libboost-system-dev libboost-filesystem-dev libglu1-mesa-dev gnuplot git openssh-server x11-apps xauth qt5-default dbus libreadline-dev wget && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install MPICH 3.0.4
RUN cd /tmp && \
    wget http://www.mpich.org/static/downloads/3.0.4/mpich-3.0.4.tar.gz && \
    tar -xzf mpich-3.0.4.tar.gz && \
    cd mpich-3.0.4 && \
    ./configure && \
    make && \
    make install && \
    ldconfig

# Verify installations
RUN gcc --version && \
    mpichversion

# Clean up
RUN rm -rf /tmp/mpich-3.0.4 /tmp/mpich-3.0.4.tar.gz

# Set up SSH keys
RUN mkdir -p /root/.ssh/
ADD ssh/* /root/.ssh/
RUN chmod 600 /root/.ssh/id_rsa
RUN chmod 600 /root/.ssh/id_ed25519
RUN chmod 600 /root/.ssh/pqdag

# Set the working directory
WORKDIR /root/
