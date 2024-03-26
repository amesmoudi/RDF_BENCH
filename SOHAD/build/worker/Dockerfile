FROM sohad_base:latest
RUN echo "export JAVA_OPTS=\"$JAVA_OPTS -Dhttp.proxyHost=cache.univ-poitiers.fr -Dhttp.proxyPort=3128  -Dhttps.proxyHost=cache.univ-poitiers.fr -Dhttps.proxyPort=3128\"">> /root/.bashrc 
RUN echo "export http_proxy=http://cache.univ-poitiers.fr:3128">> /root/.bashrc 
RUN echo "export https_proxy=http://cache.univ-poitiers.fr:3128">> /root/.bashrc 
CMD ["/usr/sbin/sshd", "-D"]



