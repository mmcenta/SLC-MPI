#ifndef DISTRIBUTED_MST_SOLVER_H
#define DISTRIBUTED_MST_SOLVER_H

#include "edge_comm.h"
#include "graph/graph.h"


class DistributedMSTSolver {
  protected:
    int rank_;
    int num_procs_;

    // comm defines and initializes edge-related structures for MPI communication
    Comm::EdgeHelper helper;

    Graph local_graph_;
    std::vector<Edge> mst_;
    double cost_;
    double elapsed_time_;
    
  public:
    DistributedMSTSolver(Graph graph);

    virtual void solve() = 0;
    double get_elapsed_time();
    double get_cost();
    std::vector<Edge> get_MST();
};

#endif // DISTRIBUTED_MST_SOLVER_H