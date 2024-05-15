
docker run                                \
    -it                                   \
    -v $(pwd):/$(basename $(pwd))         \
    -w /                                  \
    vincenzo/ubuntu-es-zybo:18.04         \
    bash
