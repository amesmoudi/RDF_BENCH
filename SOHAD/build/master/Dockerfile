FROM sohad_base:latest
WORKDIR /root/
ADD ./scripts/start-existingCluster.sh /root/
RUN chmod +x start-existingCluster.sh /root/
ADD ./scripts/genworkers.py /root/
ADD ./scripts/start-newCluster.sh /root/
RUN chmod +x start-newCluster.sh
ADD ./scripts/hostsProg.py /root/
RUN chmod +x hostsProg.py
COPY workspace /opt/workspace
RUN echo "export JAVA_OPTS=\"$JAVA_OPTS -Dhttp.proxyHost=cache.univ-poitiers.fr -Dhttp.proxyPort=3128  -Dhttps.proxyHost=cache.univ-poitiers.fr -Dhttps.proxyPort=3128\"">> /root/.bashrc 
RUN echo "export http_proxy=http://cache.univ-poitiers.fr:3128">> /root/.bashrc 
RUN echo "export https_proxy=http://cache.univ-poitiers.fr:3128">> /root/.bashrc 
WORKDIR /opt/workspace
CMD ["/usr/sbin/sshd", "-D"]


