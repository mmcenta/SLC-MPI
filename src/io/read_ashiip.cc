#include <iostream>
#include <string>
#include <sstream>
#include <random>

#include "io/read.h"
#include "graph/graph.h"


Graph read_ashiip(std::istream& in) {
  Graph g;
  std::string model, line, node_number;

  // Initialize the random weight generator
  std::mt19937_64 gen(time(0));
  std::uniform_real_distribution<double> dist(0.0, 10.0);

  // Get the model that was used
  in >> model;

  // Skip over the parameters depending on the model
  int num_params;
  if (model == "ER") num_params = 1;
  else if (model == "BA") num_params = 2;
  else if (model == "eBA") num_params = 4;

  for (int i = 0; i < 3 + num_params; ++i) getline(in, line);

  // Parse adjacency
  while (std::getline(in, line)) {
    int from, to;
    double weight;
    std::stringstream ss(line);

    getline(ss, node_number, '[');
    from = std::stoi(node_number);

    while (std::getline(ss, node_number, ',')) {
      to = std::stoi(node_number);
      weight = dist(gen);
      g.add_edge(from, to, weight); 
    }
  }
  
  return g;
}


