#include <iostream>

#include "io/read.h"
#include "graph/graph.h"


Graph read_edgelist(std::istream& in) {
  int num_edges;
  Graph g;

  // Get the number of edges
  in >> num_edges;

  // Parse edges
  while (num_edges--) {
    int from, to;
    double weight;

    in >> from >> to >> weight;

    g.add_edge(from, to, weight); 
  }
  
  return g;
}


