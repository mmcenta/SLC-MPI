#include <unordered_map>
#include <vector>

#include <openmpi/mpi.h>

#include "distributed/distributed_boruvka.h"
#include "graph/graph.h"
#include "util/union_find.h"
#include "algorithms/kruskal.h"


DistributedBoruvka::DistributedBoruvka(Graph graph) :
  DistributedMSTSolver(graph),
  sets(graph.vertices()), supervertices(graph.vertices()) {
    helper.init((MPI_User_function*) Comm::min_edge);
  }


std::vector<Edge> DistributedBoruvka::global_min_edges(std::vector<Edge> local_min) {
  int num_sv = local_min.size(); // the number of supervertexes

  // Transform the send vector into a contiguous array
  Comm::w_edge outbuf[num_sv];
  Comm::w_edge inbuf[num_sv];
  for (int i = 0; i < num_sv; ++i) {
    outbuf[i].weight = local_min[i].weight;
    outbuf[i].from = local_min[i].from;
    outbuf[i].to = local_min[i].to;
  }

  // Reduce to the global cheapest edges
  MPI_Allreduce(&outbuf[0], &inbuf[0], num_sv, helper.WEIGHTED_EDGE, helper.OP, MPI_COMM_WORLD);
  
  // Transform the received array into a vector
  std::vector<Edge> global_min(num_sv); 
  for (int i = 0; i < num_sv; ++i)
    global_min.emplace_back(inbuf[i].from, inbuf[i].to, inbuf[i].weight);

  return global_min;
}

void DistributedBoruvka::distributed_boruvka_step(std::vector<Edge>& edge_list) {
  // supervertex_index is a mapping from the supervertex descriptors to indexes
  std::unordered_map<int, int> supervertex_index;

  // Cheapest edge seen outbound from each supervertex.
  // Note that edge weights are initalized to WEIGHT_MAX
  std::vector<Edge> local_cheapest(supervertices.size());

  // Index the supervertices
  for (unsigned int i = 0; i < supervertices.size(); ++i)
    supervertex_index[supervertices[i]] = i;

  // Find the cheapest edge outbound from each set
  for (auto it = edge_list.begin(); it != edge_list.end(); ++it) {
    if (!sets.same_set(it->from, it->to)) {
      /* Check if this edge is cheaper than the current cheapest
         for each supervertex and update if needed */
      int from_index = supervertex_index[sets.find_set(it->from)];
      if (it->weight < local_cheapest[from_index].weight)
          local_cheapest[from_index] = *it;

      int to_index = supervertex_index[sets.find_set(it->to)];
      if (it->weight < local_cheapest[to_index].weight)
        local_cheapest[to_index] = *it;
    }
  }
  
  // Get the global cheapest edges outbound from each supervertexes
  std::vector<Edge> global_cheapest = global_min_edges(local_cheapest);

  // Connect the sets using the cheapest edges for each set
  for (unsigned int i = 0; i < global_cheapest.size(); ++i) {
    Edge e = global_cheapest[i];
    if (e.weight != WEIGHT_MAX && !sets.same_set(e.from, e.to)) {
      int supervertex_from = sets.find_set(e.from);
      int supervertex_to = sets.find_set(e.to);

      sets.union_set(supervertex_from, supervertex_to);
            
      if (rank_ == 0) {
        mst_.push_back(e);
        cost_ += e.weight;
      }

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

void DistributedBoruvka::solve() {
  double start, end;

  start = MPI_Wtime(); // start timer

  // Use the Kruskal algorithm on the local graph to select the edges that
  // will be considered during the distributed step.
  Kruskal local_mst(local_graph_);
  local_mst.solve();
  

  // edge_list represents the list of edges that are considered during the
  // boruvka steps and it is pruned to remove self loops
  std::vector<Edge> edge_list = local_mst.get_MST();

  // While the number of supervertices is decreasing
  int old_num_supervertexes;
  do {
    old_num_supervertexes = sets.num_sets();;
    distributed_boruvka_step(edge_list);
  } while (sets.num_sets() <  old_num_supervertexes);

  end = MPI_Wtime(); // end timer

  elapsed_time_ = 1000*(end - start);
}



