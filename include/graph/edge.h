#ifndef EDGE_H
#define EDGE_H

#include <cfloat> // for DBL_MAX

const double WEIGHT_MAX = DBL_MAX;

struct Edge {
  int from;
  int to;
  double weight;

  Edge() {
    from = to = -1;
    weight = WEIGHT_MAX;
  }

  Edge(int from, int to, double weight) :
    from(from), to(to), weight(weight) { }
};

inline bool operator==(const Edge& a, const Edge& b) {
  return ((a.from == b.from) && (a.to == b.to) && (a.weight == b.weight)) ||
         ((a.from == b.to) && (a.to == b.from) && (a.weight == b.weight));
}

inline bool operator!=(const Edge& a, const Edge& b) {
  return !(a == b);
}

inline bool operator<(const Edge& a, const Edge& b) {
  return (a.weight < b.weight);
}

inline bool operator>(const Edge& a, const Edge& b) {
  return (a.weight > b.weight);
}

// This is a blatant copy of boost::hash_combine
template <class T>
inline void hash_combine(size_t &seed, const T& v) {
  std::hash<T> h;
  seed ^= h(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// Specialize std::hash for Edge
namespace std {
  template <>
  struct hash<Edge> {
    size_t operator()(const Edge& e) const {
      int a, b;

      // sort from and to so the forwards and
      // backwards edge have the same hash
      if (e.from < e.to) a = e.from, b = e.to;
      else a = e.to, b = e.from;

      size_t seed = 0;
      hash_combine(seed, a);
      hash_combine(seed, b);
      hash_combine(seed, e.weight); 

      return seed;
    }
  };
};

#endif // EDGE_H