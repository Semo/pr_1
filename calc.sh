#!/bin/bash
echo "${1}:" >>/dev/stderr
/usr/bin/time -p ./montecarlo ${1} ${2}
