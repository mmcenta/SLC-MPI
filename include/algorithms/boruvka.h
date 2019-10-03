#ifndef BORUVKA_H
#define BORUVKA_H

#include <chrono>
#include <list>
#include <vector>

#include "graph/graph.h"
#include "util/union_find.h"
#include "algorithms/mst_solver.h"


class Boruvka : public MSTSolver {
  private:
    // sets represents the set of vertices represented by a supervertex
    UnionFind sets;
    // supervertices is a list containing all the supervertices at a given moment
    std::vector<int> supervertices;

  public:
    Boruvka(Graph graph) : 
      MSTSolver(graph), 
      sets(graph.vertices()), supervertices(graph.vertices()) { }

    void boruvka_step(std::vector<Edge> edge_list);
    void solve();
};

inline void Boruvka::boruvka_step(std::vector<Edge> edge_list) {
  // supervertex_index is a mapping from the supervertex descriptors to indexes
  std::unordered_map<int, int> supervertex_index;

  // Cheapest edge seen outbound from each supervertex.
  // Note that edge weights are initalized to WEIGHT_MAX
  std::vector<Edge> cheapest(supervertices.size());

  // Index the supervertices
  for (unsigned int i = 0; i < supervertices.size(); ++i)
    supervertex_index[supervertices[i]] = i;

  // Find the cheapest edge outbound from each set
  for (auto it = edge_list.begin(); it != edge_list.end(); ++it) {
    if (!sets.same_set(it->from, it->to)) {
      /* Check if this edge is cheaper than the current cheapest
         for each supervertex and update if needed */
      int from_index = supervertex_index[sets.find_set(it->from)];
      if (it->weight < cheapest[from_index].weight)
          cheapest[from_index] = *it;

      int to_index = supervertex_index[sets.find_set(it->to)];
      if (it->weight < cheapest[to_index].weight)
        cheapest[to_index] = *it;
    }
  }

  // Connect the sets using the cheapest edges for each set
  for (unsigned int i = 0; i < cheapest.size(); ++i) {
    Edge e = cheapest[i];
    if (e.weight != WEIGHT_MAX && !sets.same_set(e.from, e.to)) {
      int supervertex_from = sets.find_set(e.from);
      int supervertex_to = sets.find_set(e.to);

      sets.union_set(supervertex_from, supervertex_to);
            
      mst_.push_back(e);
      cost_ += e.weight;

      // Mark the supervertex that was absorved for removal
      int absorved = supervertex_from;
      if (sets.find_set(supervertex_from) != supervertex_to)
        absorved = supervertex_to;

      supervertices[supervertex_index[absorved]] = -1;
    }
  }

  // Remove self-loops
  auto rm_if_self_loop = std::remove_if(edge_list.begin(),
                                        edge_list.end(),
                                        [&] (Edge e) { 
                                          return sets.same_set(e.from, e.to);
                                        });

  edge_list.erase(rm_if_self_loop, edge_list.end());

  // Remove absorved supervertexes
  supervertices.erase(std::remove(supervertices.begin(),
                                  supervertices.end(),
                                  -1), supervertices.end());
}

inline void Boruvka::solve() {
  std::chrono::high_resolution_clock::time_point start, end;

  start = std::chrono::high_resolution_clock::now(); // start timer

  // edge_list represents the list of edges that are considered during the
  // boruvka steps and it is pruned to remove self loops
  std::vector<Edge> edge_list = graph_.edges();

  // While the number of supervertices is decreasing
  int old_num_supervertexes;
  do {
    old_num_supervertexes = sets.num_sets();;
    boruvka_step(edge_list);
  } while (sets.num_sets() <  old_num_supervertexes);
  
  end = std::chrono::high_resolution_clock::now(); // end timer

  std::chrono::duration<double, std::milli> dur = end - start;
  elapsed_time_ = dur.count();
}

#endif