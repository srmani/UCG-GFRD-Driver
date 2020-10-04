#!/bin/bash
cd test/DCE/
pwd
#mpirun -np 4 /home/travis/build/srmani/UCG-GFRD-Driver/lammps/src/lmp_mpi -in in.UCG
mpirun -n 1 /home/travis/build/srmani/UCG-GFRD-Driver/build/UCG-GFRD-Driver/UCG-GFRD-Driver -mdi "-role DRIVER -name driver -method MPI" : -n 4 /home/travis/build/srmani/UCG-GFRD-Driver/lammps/src/lmp_mpi -mdi "-role ENGINE -name LAMMPS -method MPI" -in in.UCG
