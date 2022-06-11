FROM gcc:latest as build

# install dependencies
RUN apt-get update
RUN apt-get install -y cmake
RUN apt-get install -y lsb-release wget software-properties-common
RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

# build the code
WORKDIR /app/build
ADD ./src /app/src

RUN cmake ../src && \
    cmake --build . && \
    ctest --output-on-failure

ENTRYPOINT ["./tool/tool"]
