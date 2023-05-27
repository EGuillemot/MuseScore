FROM ubuntu:latest
COPY . /MuseScore
RUN apt update && apt install -y wget graphviz fonts-roboto
RUN wget -O doxygen-1.8.13.linux.bin.tar.gz "https://downloads.sourceforge.net/project/doxygen/rel-1.8.13/doxygen-1.8.13.linux.bin.tar.gz?ts=gAAAAABka77qkU4muWartid0iBKAJJb2tx2HNdh0cMdkLMu-A36RnoEdTw-nxELvYmYiBFDjshHg8t5_Vab3ze7mvY6etnjlzg%3D%3D&r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fdoxygen%2Ffiles%2Frel-1.8.13%2Fdoxygen-1.8.13.linux.bin.tar.gz%2Fdownload"
RUN pwd
RUN ls
RUN tar -xzf doxygen-1.8.13.linux.bin.tar.gz
CMD doxygen-1.8.13/bin/doxygen --version
CMD cd /MuseScore && /doxygen-1.8.13/bin/doxygen Doxyfile.plugins && /doxygen-1.8.13/bin/doxygen Doxyfile
