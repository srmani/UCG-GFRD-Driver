---
layout: default
title: "UCG Example"
---

**Ultra-Coarse-Graining**

The method involves defining discrete states for each component in the simulation system and then constructing the model and force-field for each of the states. The model and the force-field can be at the all-atom or the coarse-grained (CG) level.

For example, if the reaction A <-> B is simulated at a coarse-grained level, then the UCG states are (i) reactant A and  (ii) product B. As mentioned above, for performing UCG simulations, one has to have a separate CG model for state A and state B and also, their respective force-fields.

The UCG functionality is agnostic of the way in which the force-field for each of the states is derived. In the case of CG force-fields, one can use any of the known methods (Force Matching or Relative Entropy Minimization or Inverse Boltzmann) to construct the force-field for each state. Since, the UCG functionality is implemented as a **LAMMPS** ***fix***, the developed force-field has to be given as input in a format suitable for ***LAMMPS***.

The following example illustrates the inputs for **UCG** ***fix***. 