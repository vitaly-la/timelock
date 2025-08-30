#!/bin/sh
set -eux

wget https://gmplib.org/download/gmp/gmp-6.3.0.tar.xz
tar xvf gmp-6.3.0.tar.xz
cd gmp-6.3.0
./configure
make -j"$(command -v nproc >/dev/null && nproc || echo 4)"
cp gmp.h .libs/libgmp.a mpz/.libs/libmpz.a ..
cd ..
rm -f gmp-6.3.0.tar.xz
