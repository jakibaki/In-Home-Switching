# docker build . -t in-home-switching-builder
# docker run -a stdout -a stderr -v $(pwd)/out:/In-Home-Switching/out in-home-switching-builder 
FROM devkitpro/devkita64_devkitarm

WORKDIR /In-Home-Switching

COPY Makefile .
COPY source/ ./source

ENTRYPOINT [ "make" ]