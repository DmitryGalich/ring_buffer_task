FROM ubuntu:22.04

COPY . .
WORKDIR /ring_buffer_task

RUN apt-get update
RUN apt-get install -y cmake
RUN apt-get install -y gcc
RUN apt-get install -y build-essential 
RUN apt-get install -y gdb 
RUN apt-get install -y git
RUN apt-get install -y valgrind
RUN apt-get install -y libgtest-dev
