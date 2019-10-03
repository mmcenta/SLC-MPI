#ifndef PRIM_H
#define PRIM_H

#include <ctime>
#include <unordered_set>
#include <queue>

#include "graph/graph.h"
#include "algorithms/mst_solver.h"


class Prim : public MSTSolver {
  private:
    std::unordered_set<int> not_annexed_;

  public:
    Prim(Graph graph) : MSTSolver(graph) {
      std::vector<int> vl = graph.vertices(); 
      for (auto it = vl.begin(); it != vl.end(); ++it)
        not_annexed_.insert(*it);
    }

    void connected_solve(int start);
    void solve();
};

inline void Prim::solve() {
  std::chrono::high_resolution_clock::time_point start, end;

  start = std::chrono::high_resolution_clock::now(); // start timer

  while(!not_annexed_.empty())
    connected_solve(*not_annexed_.begin());

  end = std::chrono::high_resolution_clock::now(); // end timer

  std::chrono::duration<double, std::milli> dur = end-start;
  elapsed_time_ = dur.count();
}

inline void Prim::connected_solve(int start) {
  std::vector<Edge> out; // auxiliar edge vector
  std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> out_queue; // outbound edges of this component

  // initialize by annexing the vertice start
  not_annexed_.erase(start);
  out = graph_.edges_out(start);
  for (auto it = out.begin(); it != out.end(); ++it) {
    if (not_annexed_.find(it->to) != not_annexed_.end())
      out_queue.emplace(*it); 
  }

    
  // start algorithm
  while (!out_queue.empty()) {
    Edge min_edge = out_queue.top(); // get edge with minimal weight
    out_queue.pop();

    if (not_annexed_.count(min_edge.to) > 0) {
      // if the destination is not annexed, annex it
      not_annexed_.erase(min_edge.to);
      mst_.emplace_back(min_edge);
      cost_ += min_edge.weight;

      out = graph_.edges_out(min_edge.to);
      for (auto it = out.begin(); it != out.end(); ++it) {
        if (not_annexed_.count(it->to) > 0)
          out_queue.emplace(*it);
      }
    }
  }
}

#endif // PRIM_H