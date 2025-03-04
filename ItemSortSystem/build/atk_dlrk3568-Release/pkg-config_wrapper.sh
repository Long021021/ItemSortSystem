#!/bin/sh
PKG_CONFIG_SYSROOT_DIR=/opt/atk-dlrk356x-toolchain/aarch64-buildroot-linux-gnu/sysroot
export PKG_CONFIG_SYSROOT_DIR
PKG_CONFIG_LIBDIR=/opt/atk-dlrk356x-toolchain/aarch64-buildroot-linux-gnu/sysroot/usr/lib/pkgconfig:/opt/atk-dlrk356x-toolchain/aarch64-buildroot-linux-gnu/sysroot/usr/share/pkgconfig:/opt/atk-dlrk356x-toolchain/aarch64-buildroot-linux-gnu/sysroot/usr/lib/aarch64-buildroot-linux-gnu/pkgconfig
export PKG_CONFIG_LIBDIR
exec pkg-config "$@"
