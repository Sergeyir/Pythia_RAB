# Overview

This is an example project to analyze nuclear modification factors of K* and \overline{K*} in CuAu collisions for energy 200 GeV for different centrality classes and for different pT ranges in Pythia8 for Glauber nuclear model.

# Required packages

- Root6 https://root.cern/
- Pythia8 https://pythia.org/

# Installation

To download run

```sh
git clone https://github.com/Sergeyir/Pythia_RAB
```

Before running macros create input and output directory in folder you've installed this project

```sh
mkdir -p data/RAB
mkdir output
```

# How to run

First you need to set paths in Makefile for root and pythia. Now specify your parameters for Pythia event generator for heavy ion collisison in file centr.cc. Then run it via run.sh 

```sh
./run.sh centr.cc
```

And you will get a file with ncharged distribution in output file. After that change input file name in file centr_range_ident.cc and run it via root

```sh
root centr_range_ident.cc
```

You will get a nice visualisation picture of centrality over ncharged distribution and centrality ranges in your terminal output. Copy it and paste in file centr.cc and then run it again. After that go to file RAB_from_TH2.cc and change input file name and NColls number for your collision type. And run it

```sh
root RAB_from_TH2.cc
```

You will get RAB graph as .png picture in output directory.
