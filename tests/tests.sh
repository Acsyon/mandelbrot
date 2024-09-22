#!/bin/sh

for fname in *.c ; do
    fname=${fname%.*}
    c99 ${fname}.c -lm -o ${fname}
    ./${fname}
    rm ${fname}
done

