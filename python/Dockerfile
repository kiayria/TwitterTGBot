FROM ubuntu:latest
MAINTAINER TwitterTGBot

RUN apt-get update -y \
    && apt-get install -y python3 python3-pip python3-dev build-essential

RUN pip3 install flask pymongo requests

WORKDIR /usr/src
COPY authorize.py ./

RUN python3 --version && ls

CMD python3 authorize.py