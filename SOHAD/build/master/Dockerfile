FROM sohad_base:latest
ADD ./config-files/workers $HADOOP_HOME/etc/hadoop/
ADD ./scripts/startup.sh .
RUN chmod +x startup.sh
ADD ./scripts/genworkers.py .
WORKDIR /opt/


