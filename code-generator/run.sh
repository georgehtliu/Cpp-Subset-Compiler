#!/bin/bash
source /u/cs241/setup
g++ -std=c++14 -g wlp4gen.cc
wlp4scan < input.wlp4 | wlp4parse | wlp4type > input.wlp4ti
./a.out < input.wlp4ti > output.asm
cs241.linkasm < output.asm > output.merl
cs241.linker output.merl print.merl alloc.merl > linked.merl
cs241.merl 0 < linked.merl > final.mips 2> /dev/null
mips.twoints final.mips