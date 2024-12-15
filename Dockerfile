FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    gcc \
    make \
    bash \
    nano \
    vim \
    git \
    && rm -rf /var/lib/apt/lists/*

SHELL ["/bin/bash", "-c"]

ARG USERNAME

RUN useradd -ms /bin/bash $USERNAME

WORKDIR /home/$USERNAME

USER $USERNAME

CMD ["bash"]
