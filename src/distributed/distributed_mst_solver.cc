#include <openmpi/mpi.h>

#include "distributed/distributed_mst_solver.h"
#include "graph/graph.h"


DistributedMSTSolver::DistributedMSTSolver(Graph graph) {
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs_);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank_);

  // Construct local graph from the graph passed by the constructor
  int local_num_vertices = graph.get_num_vertices() / num_procs_;
  std::vector<std::vector<Edge>> adj_list = graph.get_adjacency_list();

  std::vector<std::vector<Edge>>::iterator start, end;
    
  // Get iterator to the start of the local vertices
  start = adj_list.begin();
  std::advance(start, rank_*local_num_vertices);

  // Get iterator to the end of the local vertices
  if (rank_ < num_procs_-1) {
    end = adj_list.begin();
    std::advance(end, (rank_+1)*local_num_vertices);
  }
  else {
    end = adj_list.end();
  }

  // Add the local vertices to the local graph
  for (auto it = start; it != end; ++it) {
    for (auto jt = it->begin(); jt != it->end(); jt++) {
      local_graph_.add_edge(jt->from, jt->to, jt->weight);
    }
  }
}

double DistributedMSTSolver::get_elapsed_time() { return elapsed_time_; }

double DistributedMSTSolver::get_cost() { return cost_; }

std::vector<Edge> DistributedMSTSolver::get_MST() { return mst_; }
