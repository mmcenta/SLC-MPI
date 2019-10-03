#ifndef UNION_FIND_H
#define UNION_FIND_H

/* 
  This implementation of the Union Find data structure is inspired by the
book "Competitive Programming 3: The New Lower Bound of Programming Contests",
written by Steven Halim. The original implementation can be found at:
  https://github.com/stevenhalim/cpbook-code.

  Union Find data structure with path compression and rank heuristic.
*/

#include <unordered_map>
#include <vector>


class UnionFind {
  private:
    // Transform from set descriptors to indexes
    std::vector<int> sets_;
    std::unordered_map<int, int> index_of;

    // Structure related data
    std::vector<int> parent_;
    std::vector<int> rank_, set_size_;
    int num_sets_;

    // Auxiliar find for find_sets
    int find(int index);

  public:
    UnionFind() : num_sets_(0) { }
    
    UnionFind(std::vector<int> list) :
      num_sets_(0) {
      num_sets_ = 0;
      for (auto it = list.begin(); it != list.end(); ++it)
        make_set(*it);
    }

    void make_set(int element);

    int find_set(int element);
    bool same_set(int first, int second);
    void union_set(int first, int second);

    int num_sets();
    int set_size(int element_from_set);
};

inline void UnionFind::make_set(int element) {
  if (index_of.find(element) == index_of.end()) {
    int index = sets_.size(); // index of the new element

    sets_.push_back(element);
    index_of[element] = index;

    parent_.push_back(index);
    rank_.push_back(1);
    set_size_.push_back(1);

    ++num_sets_;
  }
}

inline int UnionFind::find(int index) {
  if (parent_[index] == index) 
    return index;

  parent_[index] = find(parent_[index]); // path compression

  return parent_[index];
}

inline int UnionFind::find_set(int element) {
  return sets_[find(index_of[element])];
}

inline bool UnionFind::same_set(int first, int second) {
  return (find_set(first) == find_set(second));
}

inline void UnionFind::union_set(int a, int b) {
    int a_repr = find(index_of[a]);
    int b_repr = find(index_of[b]);
    
    if (a_repr != b_repr) {
        --num_sets_;

        if (rank_[a_repr] > rank_[b_repr]) {
            parent_[b_repr] = a_repr;
            set_size_[a_repr] += set_size_[b_repr];
        }   
        else {
            parent_[a_repr] = b_repr;
            set_size_[b_repr] += set_size_[a_repr];

            if (rank_[a_repr] == rank_[b_repr]) ++rank_[b_repr];
        }
    }
}

inline int UnionFind::num_sets() { return num_sets_; }

inline int UnionFind::set_size(int element_from_set) {
    return set_size_[find(index_of[element_from_set])];
}

#endif // UNION_FIND_H