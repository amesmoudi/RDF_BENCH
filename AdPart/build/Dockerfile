FROM ubuntu:trusty

RUN sudo apt-get -y update
RUN sudo apt-get -y install nano gcc g++ build-essential mpich2 libmpich2-dev libboost-dev libboost-iostreams-dev libboost-system-dev libboost-filesystem-dev libglu1-mesa-dev gnuplot git
RUN sudo apt-get -y install openssh-server 
RUN sudo apt-get -y install qt5-default dbus
RUN mkdir -p /root/.ssh/
ADD ssh/* /root/.ssh/
RUN chmod 600 /root/.ssh/id_rsa
ADD scripts/startup.sh /root
RUN dbus-uuidgen --ensure
WORKDIR /root/
RUN git clone https://github.com/razen-alharbi/AdPart.git
WORKDIR /root/AdPart
ENV ADHASH_HOME=/root/AdPart
RUN qmake AdHashGUI.pro -r -spec linux-g++
RUN make -j4


CMD [ "sh","/root/startup.sh" ]
