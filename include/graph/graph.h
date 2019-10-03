#ifndef WEIGHTED_DIRECTED_GRAPH_H
#define WEIGHTED_DIRECTED_GRAPH_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include "graph/edge.h"


class Graph {
  private:
    int num_vertices_, num_edges_;
    std::vector<std::vector<Edge>> adj_;
    std::unordered_map<int, int> vertex_index_;
    
    int get_index(int vertex);

  public:
    Graph() : num_vertices_(0), num_edges_(0) { }

    Graph(int num_vertices) :
      num_vertices_(num_vertices), adj_(num_vertices, std::vector<Edge>()) {
      num_edges_ = 0;
    }

    // Vertex methods
    std::vector<int> vertices();
    bool has_vertex(int vertex);

    // Edge methods
    void add_edge(int from, int to, double weight);
    std::vector<Edge> edges();
    std::vector<Edge> edges_out(int vertex);
    std::vector<std::vector<Edge>> get_adjacency_list();

    // Util
    int get_num_vertices();
    int get_num_edges();
    void print(std::ostream& s);
};


inline int Graph::get_index(int vertex) {
  int index;

  auto search = vertex_index_.find(vertex);
  if (search == vertex_index_.end()) {
    index = adj_.size();

    vertex_index_.insert({vertex, index});
    adj_.emplace_back();

    ++num_vertices_;
  }
  else {
    index = search->second;
  }

  return index;
}


inline std::vector<int> Graph::vertices() {
  std::vector<int> vertices;
  
  for (auto it = vertex_index_.begin(); it != vertex_index_.end(); ++it)
    vertices.push_back(it->first);

  return vertices;
}

inline bool Graph::has_vertex(int vertex) {
  return (vertex_index_.find(vertex) != vertex_index_.end());
}


inline void Graph::add_edge(int from, int to, double weight) {
  adj_[get_index(from)].emplace_back(from, to, weight);
  adj_[get_index(to)].emplace_back(to, from, weight);

  ++num_edges_;
}

inline std::vector<Edge> Graph::edges() {
  std::vector<Edge> edge_list;

  for (int i = 0; i < num_vertices_; ++i)
    for (auto it = adj_[i].begin(); it != adj_[i].end(); ++it)
      edge_list.emplace_back(*it);

  return edge_list;
}

inline std::vector<Edge> Graph::edges_out(int vertex) {
  return adj_[get_index(vertex)];
}

inline std::vector<std::vector<Edge>> Graph::get_adjacency_list() {
  return adj_;
}

inline int Graph::get_num_vertices() { return num_vertices_; }

inline int Graph::get_num_edges() { return num_edges_; }

inline void Graph::print(std::ostream& os) {
  std::vector<int> vl = vertices();
  std::sort(vl.begin(), vl.end());

  os << "n: " << num_vertices_ << ", m: " << num_edges_;
  for (auto it = vl.begin(); it != vl.end(); ++it) {
    os << "\n" << *it << ": "; 

    std::vector<Edge> out = edges_out(*it);
    for (auto jt = out.begin(); jt != out.end(); ++jt)
      os << "( " << jt->from << ", " << jt->to << ", " << jt->weight << ") ";
  }
}

#endif // WEIGHTED_UNDIRECTED_GRAPH_H