## Run the following command to build the Docker image:


docker build -t newfastencoder .

rm -r bindata outputdata;mkdir bindata outputdata

docker run -it --rm -v /$(pwd)/rawdata:/rawdata -v /$(pwd)/bindata:/bindata -v /$(pwd)/outputdata:/outputdata newfastencoder /app/NewFastEncoder/Release/NewFastEncoder /rawdata/ /bindata/watdiv100k.nt