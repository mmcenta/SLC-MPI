#ifndef EDGE_COMM_H
#define EDGE_COMM_H

#include "openmpi/mpi.h"


namespace Comm {
 /** Represents a weighted edge for MPI communication purposes.
  *  
  * This struct was created to be used with the EdgeHelper class.
  */
  typedef struct w_edge {
    double weight;
    int from;
    int to;
  } w_edge;

 /** \brief Performs element-wise minimum operation on weighted edge object arrays.
  * 
  * This function follows the MPI_User_funtion template and is intended for use
  * when creating an user-defined MPI operation.
  * This function is commutative.
  *
  * \param in The array of left opearands.
  * 
  * \param inout The array of right operands that stores the result of the
  * operation when finished.
  * 
  * \param len The length of the operand arrays.
  * 
  * \param datatype The MPI data type of the operands, assumed to always be an
  * type based on the w_edge struct.
  */
  void min_edge(w_edge *in, w_edge *inout, int *len, MPI_Datatype *datatype);

  class EdgeHelper {
    public:
      MPI_Datatype WEIGHTED_EDGE;
      MPI_Op OP;

      void init(MPI_User_function* fn) {
        // Create and commit custom MPI type
        const int block_length[] = { 1, 2 };
        const  MPI_Aint displacement[] = { offsetof(w_edge, weight), offsetof(w_edge, from) };
        const MPI_Datatype types[] = { MPI_DOUBLE, MPI_INT };

        MPI_Type_create_struct(2, block_length, displacement, types, &WEIGHTED_EDGE);  
        MPI_Type_commit(&WEIGHTED_EDGE);

        // Create custom MPI operation
        if (fn != NULL)
          MPI_Op_create(fn, true, &OP);
      }

      void free() {
        MPI_Type_free(&WEIGHTED_EDGE);
        MPI_Op_free(&OP);
      }

      ~EdgeHelper() { free(); }
  };

  inline void min_edge(w_edge *in, w_edge *inout, int *len, MPI_Datatype *datatype) {
    for (int i = 0; i < *len; ++i)
      if (inout[i].weight > in[i].weight) {
        inout[i].weight = in[i].weight;
        inout[i].from = in[i].from;
        inout[i].to = in[i].to;
      }
  }
};

#endif // EDGE_COMM_H