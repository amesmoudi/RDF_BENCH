FROM ubuntu:trusty

# Installe software-properties-common pour add-apt-repository
RUN apt-get update && apt-get install -y software-properties-common

# Ajoute le PPA pour OpenJDK
RUN add-apt-repository ppa:openjdk-r/ppa

# Met à jour les paquets et installe les logiciels nécessaires
RUN apt-get update \
    && apt-get install -y arp-scan python gnupg unzip curl nano openjdk-7-jdk python-pip ssh git openssh-server apt-transport-https ca-certificates  \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /root/.ssh/
ADD ssh/* /root/.ssh/
RUN chmod 600 /root/.ssh/id_rsa
RUN chmod 600 /root/.ssh/id_ed25519

WORKDIR /root/


#environment variables for changing JDK, HADOOP versions and directoris
ENV HADOOP_VER=2.6.0
ENV HADOOP_ROOT_VER=2.6
ENV SPARK_VER=1.5.0
ENV SCALA_VER=2.10
ENV HIVE_VER=1.1.0
ENV JDK_TAR_NAME=jdk.tar.gz
ENV HADOOP_TAR_NAME=hadoop.tar.gz
ENV SPARK_TAR_NAME=spark.tgz
ENV HIVE_TAR_NAME=hive.tar.gz

#***setup JDK***#
WORKDIR /opt
#ADD ./ressources/${JDK_TAR_NAME} /opt/
#ADD automatically untars

#add path variables for JDK
ENV JAVA_HOME /usr/lib/jvm/java-7-openjdk-amd64
ENV PATH=$PATH:$JAVA_HOME:$JAVA_HOME/bin



#***setup hadoop***#
RUN wget --no-check-certificate https://archive.apache.org/dist/hadoop/core/hadoop-${HADOOP_VER}/hadoop-${HADOOP_VER}.tar.gz
RUN tar xvfz hadoop-${HADOOP_VER}.tar.gz


#Path variables and environment variables for HADOOP

ENV HADOOP_HOME=/opt/hadoop-${HADOOP_VER}
ENV HADOOP_STREAMING_JAR=${HADOOP_HOME}/share/hadoop/tools/lib/hadoop-streaming-${HADOOP_VER}.jar
ENV HADOOP_INSTALL=${HADOOP_HOME}
ENV HADOOP_MAPRED_HOME=${HADOOP_HOME}
ENV HADOOP_COMMON_HOME=${HADOOP_HOME}
ENV HADOOP_HDFS_HOME=${HADOOP_HOME}
ENV HADOOP_YARN_HOME=${HADOOP_HOME}
ENV HADOOP_COMMON_LIB_NATIVE_DIR=${HADOOP_HOME}/lib/native
ENV PATH=$PATH:${HADOOP_HOME}/sbin:${HADOOP_HOME}/bin:${JAVA_HOME}/bin
ENV HADOOP_OPTS="-Djava.library.path=${HADOOP_HOME}/lib/native"
ENV HADOOP_USER_CLASSPATH_FIRST=true
ENV HADOOP_CONF_DIR=$HADOOP_HOME/etc/hadoop
ENV YARN_CONF_DIR=$HADOOP_HOME/etc/hadoop
ENV HDFS_NAMENODE_USER="root"
ENV HDFS_DATANODE_USER="root"
ENV HDFS_SECONDARYNAMENODE_USER="root"
ENV YARN_RESOURCEMANAGER_USER="root"
ENV YARN_NODEMANAGER_USER="root"

#Scala

#ENV SCALA_HOME=/usr/share/scala-${SCALA_VER}
#ENV PATH=.:$SCALA_HOME/bin:$PATH


#***CONFIGURATION***#
#adding hadoop configuration files
ADD ./config-files/hadoop-env.sh $HADOOP_HOME/etc/hadoop/
ADD ./config-files/core-site.xml $HADOOP_HOME/etc/hadoop/
ADD ./config-files/hdfs-site.xml $HADOOP_HOME/etc/hadoop/
ADD ./config-files/yarn-site.xml $HADOOP_HOME/etc/hadoop/
ADD ./config-files/mapred-site.xml $HADOOP_HOME/etc/hadoop/

#***SPARK ***#
RUN wget --no-check-certificate https://archive.apache.org/dist/spark/spark-${SPARK_VER}/spark-${SPARK_VER}-bin-hadoop${HADOOP_ROOT_VER}.tgz
RUN tar xvfz spark-${SPARK_VER}-bin-hadoop${HADOOP_ROOT_VER}.tgz

# Path variables and environment variables for Spark 
ENV SPARK_HOME=/opt/spark-${SPARK_VER}-bin-hadoop${HADOOP_ROOT_VER}
ENV PATH=$PATH:$SPARK_HOME/bin
ENV SPARK_OPTS="--master yarn"

#adding hadoop configuration files
ADD ./config-files/spark-env.sh $SPARK_HOME/conf/
ADD ./config-files/spark-defaults.conf $SPARK_HOME/conf/


# Install SBT
# Téléchargez et installez SBT
RUN wget https://github.com/sbt/sbt/releases/download/v0.13.18/sbt-0.13.18.zip && \
    unzip sbt-0.13.18.zip -d /usr/local && \
    rm sbt-0.13.18.zip

# Ajoutez sbt au PATH
ENV PATH /usr/local/sbt/bin:$PATH

# Vérifiez l'installation de sbt
RUN sbt about




# Hive installation
RUN wget --no-check-certificate https://archive.apache.org/dist/hive/hive-1.1.0/apache-hive-${HIVE_VER}-bin.tar.gz
RUN tar xvfz apache-hive-${HIVE_VER}-bin.tar.gz
ENV HIVE_HOME=/opt/apache-hive-${HIVE_VER}-bin
ENV PATH=$PATH:$HIVE_HOME/bin
ADD ./config-files/hive-site.xml $HIVE_HOME/conf/

RUN echo "export JAVA_HOME=${JAVA_HOME}" >> /root/.bashrc \
    && echo "export HADOOP_HOME=${HADOOP_HOME}" >> /root/.bashrc \
    && echo "export SPARK_HOME=${SPARK_HOME}" >> /root/.bashrc \
    && echo "export HADOOP_CONF_DIR=${HADOOP_HOME}/etc/hadoop/" >> /root/.bashrc \
    && echo "export YARN_CONF_DIR=${HADOOP_HOME}/etc/hadoop/" >> /root/.bashrc \
    && echo "export HIVE_HOME=${HIVE_HOME}" >> /root/.bashrc \
    && echo "export PATH=\$PATH:\$JAVA_HOME/bin:\$HADOOP_HOME/bin:\$HADOOP_HOME/sbin:\$SPARK_HOME/bin:\$HIVE_HOME/bin" >> /root/.bashrc

EXPOSE 22
RUN mkdir -p /var/run/sshd
