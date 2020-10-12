#include <iostream>
#include <mpi.h>
#include <stdexcept>
#include <string.h>
#include <fstream>
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

  // Perform the simulation
  int niterations=0; //number of simulation steps
  int natoms;
  double energy;
  double ke, pe;
  double e_conv,d_conv;
  char *node_name=new char[MDI_NAME_LENGTH];
  int nnodes;
  char *cmd_name=new char[MDI_NAME_LENGTH];
  int n_commands;
  double *coords;
  double *coords_pass;
  int atom_types;
  int *types_list;
  double *mass_list;

  MDI_Conversion_Factor("atomic_unit_of_energy","kilocalorie_per_mol",&e_conv);
  MDI_Conversion_Factor("bohr","angstrom",&d_conv);

  MDI_Get_NNodes(LAMMPS_comm,&nnodes);
  cout<<"Number of nodes: "<<nnodes<<endl;
  for (int ii=0;ii<nnodes;ii++)
  {
    MDI_Get_Node(ii,LAMMPS_comm,node_name);
    cout<<ii<<' '<<node_name<<endl;
  }

  MDI_Get_NCommands("@COORDS",LAMMPS_comm,&n_commands);
  cout<<"Number of commands: "<<n_commands<<endl;
  for(int ii=0;ii<n_commands;ii++)
  {
    MDI_Get_Command("@COORDS",ii,LAMMPS_comm,cmd_name);
    cout<<ii<<' '<<cmd_name<<endl;
  }
  
  // Receive the number of atoms from the LAMMPS engine
  MDI_Send_Command("<NATOMS", LAMMPS_comm);
  MDI_Recv(&natoms, 1, MDI_INT, LAMMPS_comm);// Receive the number of atoms from the LAMMPS engine
  cout << "Number of atoms: " << natoms << endl;
  coords=new double [3*natoms];
  coords_pass=new double [3*natoms];
  types_list=new int [natoms];
  mass_list=new double [natoms];

  //Have the MM engine initialize a new MD simulation
  MDI_Send_Command("@INIT_MD", LAMMPS_comm);
  cout<<"initialize MD"<<endl;

  MDI_Send_Command("<PE", LAMMPS_comm);
  MDI_Recv(&energy, 1, MDI_DOUBLE, LAMMPS_comm);
  cout<<"Init energy: "<<energy<<endl;

  MDI_Send_Command("<COORDS",LAMMPS_comm);
  MDI_Recv(coords,3*natoms,MDI_DOUBLE,LAMMPS_comm);

  ofstream IC("Init_CoordFromLAMMPS.out");
  int ID=1;
  for(int ii=0;ii<3*natoms;ii=ii+3){
    IC<<ID<<' '<<coords[ii]*d_conv<<' '<<coords[ii+1]*d_conv<<' '<<coords[ii+2]*d_conv<<endl;
    ID=ID+1;
  }
  IC.close();

  /*ID=1;
  ofstream NC("NewCoord.out");
  for(int jj=0;jj<3*natoms;jj=jj+3)
    {
      coords_pass[jj]=(coords[jj]*d_conv+1.0)/d_conv;
      coords_pass[jj+1]=(coords[jj+1]*d_conv+1.0)/d_conv;
      coords_pass[jj+2]=(coords[jj+2]*d_conv+1.0)/d_conv;
      NC<<ID<<' '<<coords_pass[jj]*d_conv<<' '<<coords_pass[jj+1]*d_conv<<' '<<coords_pass[jj+2]*d_conv<<endl;
      ID=ID+1;
  }
  NC.close();*/

  /*MDI_Send_Command(">COORDS",LAMMPS_comm);
  MDI_Send(coords_pass,3*natoms,MDI_DOUBLE,LAMMPS_comm);
  MDI_Send_Command("<COORDS",LAMMPS_comm);
  MDI_Recv(coords,3*natoms,MDI_DOUBLE,LAMMPS_comm);
  ofstream PC("Perturb_CoordFromLAMMPS.out");
  ID=1;
  for(int ii=0;ii<3*natoms;ii=ii+3){
    PC<<ID<<' '<<coords[ii]<<' '<<coords[ii+1]<<' '<<coords[ii+2]<<endl;
    ID=ID+1;
  }
  PC.close();*/


  for(int ii=0;ii<=niterations;ii++)
  {
    MDI_Send_Command("@",LAMMPS_comm);
    MDI_Send_Command("<@",LAMMPS_comm);
    MDI_Recv(node_name, MDI_NAME_LENGTH, MDI_CHAR, LAMMPS_comm);
    cout<<"node name: "<<node_name<<endl;

    MDI_Send_Command("<ENERGY", LAMMPS_comm);
    MDI_Recv(&energy, 1, MDI_DOUBLE, LAMMPS_comm);

    MDI_Send_Command("<KE", LAMMPS_comm);
    MDI_Recv(&ke, 1, MDI_DOUBLE, LAMMPS_comm);
   
    MDI_Send_Command("<PE", LAMMPS_comm);
    MDI_Recv(&pe, 1, MDI_DOUBLE, LAMMPS_comm);
 
    MDI_Send_Command("@COORDS", LAMMPS_comm);
    MDI_Send_Command("<@",LAMMPS_comm);
    MDI_Recv(node_name, MDI_NAME_LENGTH, MDI_CHAR, LAMMPS_comm);
    cout<<"node name: "<<node_name<<endl;

    cout<<"iteration: "<<ii<<' '<<energy*e_conv<<' '<<ke*e_conv<<' '<<pe*e_conv<<endl;
  }
  
  // Send the "EXIT" command to each of the engines
  MDI_Send_Command("EXIT", LAMMPS_comm);

  // Synchronize all MPI ranks
  MPI_Barrier(world_comm);
  return 0;
}
