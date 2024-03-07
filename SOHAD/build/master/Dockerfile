FROM sohad_base:latest
WORKDIR /root/
ADD ./scripts/start-existingCluster.sh /root/
RUN chmod +x start-existingCluster.sh /root/
ADD ./scripts/genworkers.py /root/
ADD ./scripts/start-newCluster.sh /root/
RUN chmod +x start-newCluster.sh
ADD ./scripts/hostsProg.sh /root/
RUN chmod +x hostsProg.sh
CMD ["/usr/sbin/sshd", "-D"]


