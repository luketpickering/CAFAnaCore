#!/bin/bash

if [ ! -e cmdstan ]; then
  git clone --branch v2.38.0 --depth 1 https://github.com/stan-dev/cmdstan.git --recursive
fi

if [ ! -e cmdstan/bin/stanc ]; then
  cd cmdstan
  make build -j 10
  cd ..
fi

STAN_MATH_DIR=$(readlink -f cmdstan/stan/lib/stan_math)
echo "export STAN_MATH_DIR=${STAN_MATH_DIR}" > stan.env

# determine the version of stanmath
cat << EOF > stanmathv.cpp
#include "stan/math/version.hpp"
#include <iostream>
int main(){ std::cout << STAN_MATH_MAJOR << "." << STAN_MATH_MINOR << "." << STAN_MATH_PATCH; }
EOF
g++ stanmathv.cpp -I${STAN_MATH_DIR} -o stanmathv

STAN_MATH_VERSION=$(./stanmathv)
rm stanmathv stanmathv.cpp
echo "export STAN_MATH_VERSION=${STAN_MATH_VERSION}" >> stan.env
echo "export STAN_MATH_INC=${STAN_MATH_DIR}" >> stan.env

echo "export BOOST_INC=$(readlink -f cmdstan/stan/lib/stan_math/lib/boost_*)" >> stan.env
echo "export EIGEN_INC=$(readlink -f cmdstan/stan/lib/stan_math/lib/eigen_*)" >> stan.env

SUNDIALS_DIR=$(readlink -f cmdstan/stan/lib/stan_math/lib/sundials_*)
SUNDIALS_BNAME=$(basename ${SUNDIALS_DIR})

echo "export SUNDIALS_DIR=${SUNDIALS_DIR}" >> stan.env
echo "export SUNDIALS_VERSION=${SUNDIALS_BNAME##*_}" >> stan.env
echo "export SUNDIALS_INC=${SUNDIALS_DIR}/include" >> stan.env
echo "export SUNDIALS_LIB=${SUNDIALS_DIR}/lib" >> stan.env

TBB_DIR=$(readlink -f cmdstan/stan/lib/stan_math/lib/tbb_*)
TBB_BNAME=$(basename ${TBB_DIR})

echo "export TBB_DIR=${TBB_DIR}" >> stan.env
echo "export TBB_VERSION=${TBB_BNAME##*_}" >> stan.env
echo "export TBB_INC=${TBB_DIR}/include" >> stan.env
echo "export TBB_LIB=$(readlink -f cmdstan/stan/lib/stan_math/lib/tbb/lib)" >> stan.env

echo "export STAN_MATH_LOCAL=1" >> stan.env
