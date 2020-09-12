ARG UBUNTU_VERSION=xenial
FROM ubuntu:${UBUNTU_VERSION} AS builder

## Deps
RUN apt-get update && apt-get install -yq \
    autoconf \
    automake \
    g++ \
    git \
    gcc \
    make \
    texinfo \
    wget
RUN apt-get clean

## Environment
ENV BASE_DIR /sources
ENV XEN_ROOT ${BASE_DIR}/xen
ENV TOOLCHAIN_ROOT ${BASE_DIR}/toolchain
ENV MINIOS_ROOT ${BASE_DIR}/minios
ENV CLICKOS_ROOT ${BASE_DIR}/clickos

## Xen Hypervisor Code
ARG XEN_BR=stable-4.14
ARG XEN_REPO=https://github.com/xen-project/xen
# Cache Break
ARG XEN_CACHE_TWEAK="Add the date to this arg to break the cache here"
RUN git clone -b ${XEN_BR} ${XEN_REPO} ${XEN_ROOT}

## Sysml Toolchain
ARG TOOLCHAIN_BR=master
ARG TOOLCHAIN_REPO=https://github.com/sysml/toolchain
# Cache Break
ARG TOOLCHAIN_CACHE_TWEAK="Add the date to this arg to break the cache here"
RUN git clone -b ${TOOLCHAIN_BR} ${TOOLCHAIN_REPO} ${TOOLCHAIN_ROOT}

## MiniOS
ARG MINIOS_BR=master
ARG MINIOS_REPO=https://github.com/sysml/mini-os
# Cache Break
ARG MINIOS_CACHE_TWEAK="Add the date to this arg to break the cache here"
RUN git clone -b ${MINIOS_BR} ${MINIOS_REPO} ${MINIOS_ROOT}

## Build Toolchain
WORKDIR ${TOOLCHAIN_ROOT}
RUN make -j all 
ENV LWIP_ROOT "${TOOLCHAIN_ROOT}/x86_64-root/x86_64-xen-elf"
ENV NEWLIB_ROOT "${TOOLCHAIN_ROOT}/x86_64-root/x86_64-xen-elf"

COPY . ${CLICKOS_ROOT}

## Build ClickOS
ARG EXTRA_FLAGS=""
ARG STATS_LEVEL=0
WORKDIR ${CLICKOS_ROOT}
RUN ./configure --with-xen=${XEN_ROOT} \
                --with-minios=${MINIOS_ROOT} \
                --with-newlib=${NEWLIB_ROOT} \
                --with-lwip=${LWIP_ROOT} \
                --enable-minios \
                --enable-stats=${STATS_LEVEL} \
                ${EXTRA_FLAGS}
RUN make -j elemlist
RUN make -j $(getconf _NPROCESSORS_ONLN) minios
RUN mkdir -p /out \
 && mv ${CLICKOS_ROOT}/minios/build/* /out

## Multi-Stage Niceness
FROM alpine:latest

COPY --from=builder /out/clickos_x86_64* /clickos/
RUN mkdir -p /output

ENTRYPOINT [ "cp", "-R", "/clickos", "/output" ]