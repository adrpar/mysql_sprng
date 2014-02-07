#include "sprng.h"
#include "../globals.h"

using namespace std;

void get_proc_info_mpi(int *myid, int *nprocs)
{
	*myid = myId;
	*nprocs = numNodes;	
}

int make_new_seed_mpi(void)
{
#ifdef SPRNG_MPI
  unsigned int temp2;
  int myid, nprocs;
  MPI_Comm newcomm;
  
  MPI_Comm_dup(MPI_COMM_WORLD, &newcomm); /* create a temporary communicator */
  
  MPI_Comm_rank(newcomm, &myid);
  MPI_Comm_size(newcomm, &nprocs);

  if(myid == 0)
    temp2 = make_new_seed();
  
  MPI_Bcast(&temp2,1,MPI_UNSIGNED,0,newcomm);

  MPI_Comm_free(&newcomm);
  
  return temp2;
#else
  return make_new_seed();
#endif
}
