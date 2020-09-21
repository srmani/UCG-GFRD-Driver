#!/bin/bash
cd test/DCE/
pwd
mpirun -np 4 /home/travis/build/srmani/UCG-GFRD-Driver/lammps/src/lmp_mpi -in in.UCG
