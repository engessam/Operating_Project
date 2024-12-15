#!/bin/bash

USERNAME=$(whoami)

docker build -t os-course-env --build-arg USERNAME="$USERNAME" .

docker run -it \
  -v "$MOUNTED_DIR:/home/$USERNAME/mounted_dir" \
  -e USERNAME="$USERNAME" \
  os-course-env
