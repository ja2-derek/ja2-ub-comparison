#!/bin/bash

diff -rw -U 0 src_ub/game/ src_ja2/game/ | grep ^@ | wc -l 
