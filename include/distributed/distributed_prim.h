#ifndef DISTRIBUTED_PRIM_H
#define DISTRIBUTED_PRIM_H

#include <queue>
#include <unordered_set>

#include "distributed_mst_solver.h"
#include "graph/graph.h"


class DistributedPrim : public DistributedMSTSolver {
  private:
    std::unordered_set<int> not_annexed_;

  public:
    typedef std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>>
                                                         min_edge_priority_queue;

    DistributedPrim(Graph graph);

    void annex_vertex(int vertex, min_edge_priority_queue& local_queue);
    bool distributed_prim_step(min_edge_priority_queue& local_queue);
    void connected_solve(int start);
    void solve();
};

#endif // DISTRIBUTED_PRIM_H