#include <queue>
#include <unordered_map>
#include <vector>

#include <openmpi/mpi.h>

#include "distributed/distributed_prim.h"
#include "graph/graph.h"


DistributedPrim::DistributedPrim(Graph graph) :
  DistributedMSTSolver(graph) {
    helper.init((MPI_User_function*) Comm::min_edge);

    std::vector<int> vl = graph.vertices();
    for (auto it = vl.begin(); it != vl.end(); ++it)
      not_annexed_.insert(*it);
  }


void DistributedPrim::solve() {
  double start, end;

  start = MPI_Wtime(); // start timer

  while (!not_annexed_.empty()) {
    connected_solve(*not_annexed_.begin());
  }

  end = MPI_Wtime(); // end timer

  elapsed_time_ = 1000*(end-start);
}

void DistributedPrim::annex_vertex(int vertex, 
                                   min_edge_priority_queue& local_queue) {
  not_annexed_.erase(vertex);
  
  if (local_graph_.has_vertex(vertex)) {
      // Get outbound edges
      auto out = local_graph_.edges_out(vertex);

      for (auto it = out.begin(); it != out.end(); ++it)
        if (not_annexed_.find(it->to) != not_annexed_.end())
          local_queue.push(*it);
  }
}


bool DistributedPrim::distributed_prim_step(min_edge_priority_queue& local_queue) {
  // Get minimal edge outbound from the local annexed component
  Edge local_min;
  do {
    if (local_queue.empty()) {
      local_min.weight = WEIGHT_MAX;
      local_min.from = local_min.to = -1;
    }
    else {
      local_min = local_queue.top();
      local_queue.pop();
    }
  } while (not_annexed_.find(local_min.to) == not_annexed_.end() &&
           local_min.weight != WEIGHT_MAX);

  // Create MPI buffer and assign it to the local minimum edge
  Comm::w_edge outbuf, inbuf;
  outbuf.weight = local_min.weight;
  outbuf.from = local_min.from;
  outbuf.to = local_min.to;

  // Reduce to get the global minimum
  MPI_Allreduce(&outbuf, &inbuf, 1, helper.WEIGHTED_EDGE, helper.OP, MPI_COMM_WORLD);
  if (inbuf.weight == WEIGHT_MAX) // if all processors have empty queues
    return true;

  // Convert the buffer to an Edge object 
  Edge global_min(inbuf.from, inbuf.to, inbuf.weight);

  // Annex the next vertex
  annex_vertex(global_min.to, local_queue);

  // If the the local min was not the global min, re-insert the
  // local min into the queue
  if (local_min.weight != WEIGHT_MAX && local_min != global_min)
    local_queue.emplace(local_min);
  
  // If this is the root proccess, update the MST
  if (rank_ == 0) {
    mst_.emplace_back(global_min);
    cost_ += global_min.weight;
  }

  return false;
}

void DistributedPrim::connected_solve(int start) {
  min_edge_priority_queue local_queue; 

  // Broadcast starting vertex
  MPI_Bcast(&start, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Annex starting vertex
  annex_vertex(start, local_queue);

  // start algorithm
  bool stop = false;
  while (!stop) {
    stop = distributed_prim_step(local_queue);
    MPI_Barrier(MPI_COMM_WORLD);
  }
}
