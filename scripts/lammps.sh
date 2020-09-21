#!/bin/bash
#git clone -b mdi https://github.com/MolSSI-MDI/lammps.git
git clone -b stable https://github.com/lammps/lammps.git
cd lammps
cd src
#cp ../../src/fix_ucg_gfrd.cpp .
#cp ../../src/fix_ucg_gfrd.h .
#make lib-mdi args="-m gcc"
#make yes-user-mdi
make yes-MOLECULE
make yes-MC
make yes-RIGID
make yes-kspace
make mpi
cd ../../
pwd
