FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-22.04

RUN apt-get update \
    && apt-get install software-properties-common -y \
    && add-apt-repository ppa:deadsnakes/ppa -y \
    && apt-get install -y g++ make git docker.io cmake build-essential zsh python3.11 python3-pip fish \
    && curl -L https://github.com/oh-my-fish/oh-my-fish/raw/master/bin/install > install \
    && fish install  \
    && rm install \
    && chsh -s /usr/bin/fish \
    && omf install eclm \
    && git config --global core.editor "nano"