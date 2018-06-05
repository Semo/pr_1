#!/bin/bash
echo "${1}:" >>/dev/stderr
/usr/bin/time -p ./pi_omp ${1} ${2}
