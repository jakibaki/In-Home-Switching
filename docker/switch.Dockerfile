# docker build . -t in-home-switching-switch-builder -f docker/Dockerfile
# docker run -a stdout -a stderr in-home-switching-switch-builder
FROM devkitpro/devkita64_devkitarm

WORKDIR /app/switch

COPY Makefile .
COPY source/ ./source

ENTRYPOINT ["make"]