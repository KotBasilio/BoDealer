FROM python:stretch
COPY . .

# dev tools
RUN apt-get update
RUN apt-get install -y nano
RUN apt-get install -y ffmpeg

# common
RUN apt-get install -y wget
RUN apt-get install -y unzip

# offline translation
RUN apt-get install -y apt-transport-https
RUN apt-get install -y libicu-dev
RUN apt-get install -y build-essential
