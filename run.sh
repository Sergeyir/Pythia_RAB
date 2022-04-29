#!/bin/bash

name=$1

make ${name} && ./${name} 1

