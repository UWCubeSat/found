FROM gcc:latest
RUN apt update
RUN apt -y upgrade
RUN apt install -y cmake
RUN apt install -y wget
RUN apt install -y tar
RUN apt install -y valgrind
RUN apt install -y python3
RUN apt install -y cpplint
RUN apt install -y gcovr
RUN apt install -y doxygen
RUN apt install -y graphviz