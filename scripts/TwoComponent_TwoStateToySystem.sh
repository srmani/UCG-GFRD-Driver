#!/bin/bash
cd test/TwoComponent_TwoStateToySystem/
mpirun -np 6 /home/travis/build/srmani/UCG-GFRD/lammps/src/lmp_mpi -in in.UCG