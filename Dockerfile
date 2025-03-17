FROM ubuntu:latest
RUN apt update
RUN apt -y upgrade
RUN apt install -y git
RUN apt install -y g++
RUN apt install -y cmake
RUN apt install -y wget
RUN apt install -y tar
RUN apt install -y valgrind
RUN apt install -y python3
RUN apt install -y python3-pip
RUN pip install --break-system-packages git+https://github.com/cpplint/cpplint.git@2.0.0#egg=cpplint
RUN pip install --break-system-packages git+https://github.com/gcovr/gcovr.git@8.3#egg=gcovr
RUN apt install -y doxygen
RUN apt install -y graphviz
