FROM ubuntu:latest
MAINTAINER TwitterTGBot

RUN echo "debconf debconf/frontend select Noninteractive" | debconf-set-selections
RUN apt-get -qq update && apt-get -qq install -y \
    g++ \
    make \
    binutils \
    cmake \
    libssl-dev \
    libboost-all-dev \
    libcurl4-openssl-dev \
    zlib1g-dev \
    git \
    wget \
    tar \
    && rm -rf /var/lib/apt/lists/*

# build libson     libbson-1.0
WORKDIR /usr/src/temp_libbson
RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.17.0-rc0/mongo-c-driver-1.17.0-rc0.tar.gz \
    && tar xzf mongo-c-driver-1.17.0-rc0.tar.gz \
    && cd mongo-c-driver-1.17.0-rc0 \
    && mkdir cmake-build \
    && cd cmake-build \
    && cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. \
    && make install \
    && rm -rf /usr/src/temp_libbson


# mongocxx
WORKDIR /usr/src/temp
RUN git clone https://github.com/mongodb/mongo-cxx-driver.git --branch releases/stable --depth 1 \
    && cd mongo-cxx-driver/build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local \
    && make EP_mnmlstc_core \
    && make \
    &&  make install \
    && rm -rf /usr/src/temp


WORKDIR /usr/src/TwitterTGBot
COPY src src
COPY tgbot-cpp tgbot-cpp
COPY Twitter-API-C-Library Twitter-API-C-Library
COPY keys ./
COPY CMakeLists.txt ./
COPY main.cpp ./

#build ./TwitterTGBot
RUN mkdir ./build \
    && cmake ./build . && make

CMD ["./TwitterTGBot"]