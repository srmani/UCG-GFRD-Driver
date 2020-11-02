#include <iostream>
#include <mpi.h>
#include <stdexcept>
#include <string.h>
#include "mdi.h"

using namespace std;

int main(int argc, char **argv) {

  // Initialize the MPI environment
  MPI_Comm world_comm;
  MPI_Init(&argc, &argv);

  // Read through all the command line options
  int iarg = 1;
  bool initialized_mdi = false;
  while ( iarg < argc ) {

    if ( strcmp(argv[iarg],"-mdi") == 0 ) {

      // Ensure that the argument to the -mdi option was provided
      if ( argc-iarg < 2 ) {
        throw runtime_error("The -mdi argument was not provided.");
      }

      // Initialize the MDI Library
      world_comm = MPI_COMM_WORLD;
      int ret = MDI_Init(argv[iarg+1], &world_comm);
      if ( ret != 0 ) {
        throw runtime_error("The MDI library was not initialized correctly.");
      }
      initialized_mdi = true;
      iarg += 2;

    }
    else {
      throw runtime_error("Unrecognized option.");
    }

  }
  if ( not initialized_mdi ) {
    throw runtime_error("The -mdi command line option was not provided.");
  }

  // Connect to the engines {LAMMPS and GFRD: engine names}
  // following lines will change to incorporate GFRD
  MDI_Comm LAMMPS_comm=MDI_COMM_NULL;
  int nengines = 1;  
  for (int iengine=0; iengine < nengines; iengine++)
  {
    MDI_Comm comm;
    MDI_Accept_Communicator(&comm);

    // Determine the name of this engine
    char* engine_name = new char[MDI_NAME_LENGTH];
    MDI_Send_Command("<NAME", comm);
    MDI_Recv(engine_name, MDI_NAME_LENGTH, MDI_CHAR, comm);
 
    cout << "Engine name: " << engine_name << endl;

    if ( strcmp(engine_name, "LAMMPS") == 0 ) {
      if ( LAMMPS_comm != MDI_COMM_NULL ) {
	throw runtime_error("Accepted a communicator from a second LAMMPS engine. Check the number of LAMMPS input calls");
      }
      LAMMPS_comm = comm;
    }
    /* else if ( strcmp(engine_name, "GFRD") == 0 ) {
      if ( GFRD_comm != MDI_NULL_COMM ) {
	throw runtime_error("Accepted a communicator from a second GFRD engine. Check the number of GFRD input calls");
      }
      GFRD_comm = comm;
      }*/
    else {
      throw runtime_error("Unrecognized engine name.");
    }
    delete[] engine_name;
  }
  
  int exists;

  try
  {
    MDI_Check_Node_Exists("@DEFAULT", LAMMPS_comm, &exists);
    if (exists!=1)
      throw (0);
  }

  catch (...)
  {
    return 1;
  }

  return 0;
  
  // Send the "EXIT" command to each of the engines
  MDI_Send_Command("EXIT", LAMMPS_comm);

  // Synchronize all MPI ranks
  MPI_Barrier(world_comm);
  MPI_Finalize();
  return 0;
}
