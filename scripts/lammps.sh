#!/bin/bash
git clone -b mdi https://github.com/MolSSI-MDI/lammps.git
git clone https://github.com/srmani/UCG-GFRD.git
cd lammps
cd src
cp ../../UCG-GFRD/src/fix_ucg_gfrd.cpp .
cp ../../UCG-GRRD/src/fix_ucg_gfrd.h .
#make lib-mdi args="-m gcc"
#make yes-user-mdi
make yes-MOLECULE
make yes-MC
make yes-RIGID
make yes-kspace
make mpi
cd ../../
pwd
