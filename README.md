# Balls of Steel

Balls of steel (BOS) is an event-driven, hard-particles discrete particle method (DPM) simulator.
BoS aims to be a minimal, extremely fast but easily extendable version of an even-driven (ED) simulator.
It is currently in initial development.

## Installation

Use cmake to create makefiles.
Then just 'make'.
It is recommended that you build in a separate directory.
For example, from the source code parent directory:
'''
mkdir build
cd build
ccmake ../
'''
then choose the desired ccmake options, hit g and then c. If no errors were reported,
'''
make
'''
should provide you with a 'bos' executable.

## Usage
Run as:

> ./bos 'config-file'

where config-file is expected to be a [DEMDF](https://github.com/nicorivas/demdf) file.
