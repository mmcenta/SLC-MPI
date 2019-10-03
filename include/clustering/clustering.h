#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <algorithm> // for std::sort
#include <vector>

#include "graph/graph.h"
#include "graph/edge.h"
#include "algorithms/kruskal.h"
#include "util/union_find.h"


class Clustering { 
  private:
    int k_;

    MSTSolver *solver_;
    UnionFind clusters_;

    void explore_cluster(int start);

  public:
    Clustering(Graph& graph, int k) : k_(k), clusters_(graph.vertices()) { 
      solver_ = &Kruskal(graph);
    }

    void cluster();
    UnionFind get_clusters();
};

inline void Clustering::cluster() {
  std::vector<Edge> tree;

  solver_->solve();
  tree = solver_->get_MST();
  
  for (int i = 0; i < k_; i++) {
    tree.pop_back();
  }

  // Get the connected components
  for (auto it = tree.begin(); it != tree.end(); it++)
    clusters_.union_set(it->from, it->to);
}

inline UnionFind Clustering::get_clusters() {
  return clusters_;
}

#endif // CLUSTERING_H