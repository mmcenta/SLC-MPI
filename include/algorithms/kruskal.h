#ifndef KRUSKAL_H
#define KRUSKAL_H

#include <chrono>
#include <algorithm> // for std::sort

#include "graph/graph.h"
#include "util/union_find.h"
#include "algorithms/mst_solver.h"


class Kruskal : public MSTSolver { 
  public:
    Kruskal(Graph graph) : MSTSolver(graph) { }
    void solve();
};

inline void Kruskal::solve() {
  std::chrono::high_resolution_clock::time_point start, end;

  start = std::chrono::high_resolution_clock::now();; // start timer

  // sets represents the connected components
  UnionFind sets(graph_.vertices());
  // all_edges contains all edges on the graph
  std::vector<Edge> all_edges = graph_.edges();

  // sort the edges 
  std::sort(all_edges.begin(), all_edges.end());

  int num_chosen_edges = 0;
  unsigned int i = 0;
  while (i < all_edges.size() && 
         num_chosen_edges < graph_.get_num_vertices()-1) {
    Edge curr = all_edges[i++];
    if (!sets.same_set(curr.from, curr.to)) {
      ++num_chosen_edges;
      sets.union_set(curr.from, curr.to);
      
      mst_.emplace_back(curr);
      cost_ += curr.weight;
    }
  }

  end = std::chrono::high_resolution_clock::now(); // end timer

  std::chrono::duration<double, std::milli> dur = end-start;
  elapsed_time_ = dur.count();
}

#endif