#ifndef MST_SOLVER_H
#define MST_SOLVER_H

#include <vector>

#include "graph/graph.h"


class MSTSolver {
  protected:
    Graph graph_;
    std::vector<Edge> mst_;
    double cost_;
    double elapsed_time_;

  public:
    MSTSolver(Graph graph) : graph_(graph) {
      elapsed_time_ = 0;
      cost_ = 0;
    }

    virtual void solve() = 0;
    double get_elapsed_time();
    double get_cost();
    std::vector<Edge> get_MST();
};

inline double MSTSolver::get_elapsed_time() { return elapsed_time_; }

inline double MSTSolver::get_cost() { return cost_; }

inline std::vector<Edge> MSTSolver::get_MST() { return mst_; }

#endif