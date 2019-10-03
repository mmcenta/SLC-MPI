#ifndef DISTRIBUTED_BORUVKA_H
#define DISTRIBUTED_BORUVKA_H

#include <vector>

#include "distributed_mst_solver.h"
#include "edge_comm.h"
#include "graph/graph.h"
#include "util/union_find.h"


class DistributedBoruvka : public DistributedMSTSolver {
  private:
    // sets represents the set of vertices represented by a supervertex
    UnionFind sets;
    // supervertices is a list containing all the supervertices at a given moment
    std::vector<int> supervertices;

  public:
    DistributedBoruvka(Graph graph);

    std::vector<Edge> global_min_edges(std::vector<Edge> local_edge);  
    void distributed_boruvka_step(std::vector<Edge>& edge_list);
    void solve();
};


#endif // DISTRIBUTED_BORUVKA_H