#!/usr/bin/env bash

gcc main.c -o pstreeFS -D_FILE_OFFSET_BITS=64 -lfuse

