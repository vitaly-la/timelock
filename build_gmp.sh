#!/bin/sh
set -eux
IFS=$'\n\t'

if [ -z "${CC+x}" ]; then
  echo 'Please set compiler, e.g. export CC=gcc'
  exit 1
fi

wget https://gmplib.org/download/gmp/gmp-6.3.0.tar.xz
tar xvf gmp-6.3.0.tar.xz
cd gmp-6.3.0
./configure
make -j4
cp gmp.h .libs/libgmp.a mpz/.libs/libmpz.a ..
cd ..
rm -f gmp-6.3.0.tar.xz
