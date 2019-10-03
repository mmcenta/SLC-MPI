#ifndef GHS_COMM_H
#define GHS_COMM_H

#include <openmpi/mpi.h>


namespace Comm {

	enum MessageType {
		ACCEPT = 0,
		REJECT,
		CONNECT,
		REPORT,
		TEST,
		INIT,
		CHANGE_CORE,
		NONE
	};

  struct ghs_message {
    int type;
    int first;
    double second;
    int third;
  };

  class GHSHelper {
    public:
      MPI_Datatype MESSAGE;
      MPI_Op OP;

      void init(MPI_User_function *fn) {
        // Create and commit custom MPI type
				const int block_size[] = { 2, 1, 1 };
				const MPI_Aint displacement[] = { offsetof(ghs_message, type),
																					offsetof(ghs_message, second),
																					offsetof(ghs_message, third) };
				const MPI_Datatype type[] = { MPI_INT, MPI_DOUBLE, MPI_INT };

				MPI_Type_create_struct(3, block_size, displacement, type, &MESSAGE);
        MPI_Type_commit(&MESSAGE);

        // Create custom MPI operation
				if (fn != NULL)
        	MPI_Op_create(fn, true, &OP);
      }
	};

};

#endif // GHS_COMM_H