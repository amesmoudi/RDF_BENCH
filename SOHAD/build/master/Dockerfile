FROM sohad_base:latest
WORKDIR /root/
ADD ./scripts/start-existingCluster.sh /root/
RUN chmod +x start-existingCluster.sh /root/
ADD ./scripts/genworkers.py /root/
ADD ./scripts/start-newCluster.sh /root/
RUN chmod +x start-newCluster.sh
ADD ./scripts/hostsProg.py /root/
RUN chmod +x hostsProg.py
ADD ./scripts/loadData.sh /root/
RUN chmod +x loadData.sh
ADD ./scripts/replaceNS.py /root/
COPY workspace /opt/workspace
RUN mkdir /data/
RUN apt-get update
RUN apt-get install -y screen
WORKDIR /opt/workspace
CMD ["/usr/sbin/sshd", "-D"]


