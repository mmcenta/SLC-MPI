#include <vector>

#include "graph/graph.h"
#include "distributed/ghs_comm.h"
#include "distributed/ghs_node.h"


GHSNode::GHSNode(std::vector<Edge> adjacent_edges) {
  state = SLEEPING;
  halt_flag = 0;
  
  id = adjacent_edges.front().from;
  for (auto it = adjacent_edges.begin(); it != adjacent_edges.end(); ++it) {
    edges.push_back(it->to);
    edge_weights[it->to] =it->weight;
    edge_states[it->to] = BASIC;
  }
}

void GHSNode::find_best_basic(int& best_basic_edge, double& best_basic_wt) {
  // Note that for the GHS algorithm we must break ties consistently
  best_basic_wt = WEIGHT_MAX;
  for (auto it = edges.begin(); it != edges.end(); ++it)
    if (edge_states[*it] == BASIC &&
        (edge_weights[*it] < best_basic_wt ||
         (edge_weights[*it] == best_basic_wt && *it < best_basic_edge))) {
      best_basic_wt = edge_weights[*it];
      best_basic_edge = *it;
  }
}

void GHSNode::wakeup() {
  // Find the edge with minimum weight
  find_best_basic(best_edge, best_wt);

  // Modify local variables
  edge_states[best_edge] = BRANCH;
  level = 0;
  state = FOUND;
  find_count = 0;

  // Send Connect(0) through the best edge
  Comm::ghs_message msg;
  msg.type = Comm::CONNECT;
  msg.first = 0;

  send.emplace(best_edge, msg);
}

void GHSNode::respond_connect(int in_edge, int in_level) {
  if (state == SLEEPING) wakeup();

  if (in_level < level) {
    edge_states[in_edge] = BRANCH;

    // Send Initiate(level, fragment, state) on in_branch
    Comm::ghs_message msg;
    msg.type = Comm::INIT;
    msg.first = level;
    msg.second = fragment;
    msg.third = state;

    send.emplace(in_edge, msg);

    if (state == FIND) ++find_count;
  }
  else if (edge_states[in_edge] == BASIC) {
    // Place this message at the end of the inbound queue
    Comm::ghs_message msg;
    msg.type = Comm::CONNECT;
    msg.first = in_level;

    recv.emplace(in_edge, msg);
  }
  else {
    // Send Initiate(LN+1, w(in_edge), Find) on in_edge
    Comm::ghs_message msg;
    msg.type = Comm::INIT;
    msg.first = level+1;
    msg.second = edge_weights[in_edge];
    msg.third = FIND;

    send.emplace(in_edge, msg);
  }
}

void GHSNode::respond_initiate(int in_edge, int in_level, double in_fragment, EdgeState in_state) {
  level = in_level;
  fragment = in_fragment;
  in_branch = in_edge;

  best_edge = -1;
  best_wt = WEIGHT_MAX;

  // For every adjacent edge with state in Branch
  for (auto it = edges.begin(); it != edges.end(); ++it)
    if (edge_states[*it] == BRANCH) {
      // Send Initiate(L, F, S) on edge *it
      Comm::ghs_message msg;
      msg.type = Comm::INIT;
      msg.first = in_level;
      msg.second = in_fragment;
      msg.third = in_state;

      send.emplace(*it, msg);

      // If S = Find then augment find-count
      if (in_state == FIND) ++find_count;
    }
    
    if (in_state == FIND) test();
}

void GHSNode::test() {
  // Find adjacent in state Basic with minimum weight
  double test_wt;
  test_edge = -1;

  find_best_basic(test_edge, test_wt);

  // If there is such edge
  if (test_wt != WEIGHT_MAX) {
    // Send Test(LN, FN) on test-edge
    Comm::ghs_message msg;
    msg.type = Comm::TEST;
    msg.first = level;
    msg.second = fragment;

    send.emplace(test_edge, msg);
  }
  else {
    report();
  }
}

void GHSNode::respond_test(int in_edge, int in_level, double in_fragment) {
  if (state == SLEEPING) wakeup();

  if (in_level > level) {
    // Place message at the end of the incoming queue
    Comm::ghs_message msg;
    msg.type = Comm::TEST;
    msg.first = in_level;
    msg.second = in_fragment;

    send.emplace(in_edge, msg);
  }
  else if (in_fragment != fragment) {
    // Send Accept on edge in_edge
    Comm::ghs_message msg;
    msg.type = Comm::ACCEPT;

    send.emplace(in_edge, msg);
  }
  else {
    if (edge_states[in_edge] == BASIC)
      edge_states[in_edge] == REJECTED; // :(

    if (test_edge != in_edge) {
      // Send Reject on edge in_edge
      Comm::ghs_message msg;
      msg.type = Comm::REJECT;

      send.emplace(in_edge, msg);
    }
    else {
      test();
    }
  }
}

void GHSNode::respond_accept(int in_edge) {
  test_edge = -1;

  if (edge_weights[in_edge] < best_wt) {
    best_edge = in_edge;
    best_wt = edge_weights[in_edge];
  }
  
  report();
}

void GHSNode::respond_reject(int in_edge) {
  if (edge_states[in_edge] == BASIC)
    edge_states[in_edge] = REJECTED; // :(

  test();
}

void GHSNode::report() {
  if (find_count == 0 && test_edge == -1) {
    state = FOUND;

    // Send Report(best-wt) on in-branch
    Comm::ghs_message msg;
    msg.type = Comm::REPORT;
    msg.second = best_wt;

    send.emplace(in_branch, msg);
  }
}

void GHSNode::respond_report(int in_edge, double in_weight) {
  if (in_weight == WEIGHT_MAX && best_wt == WEIGHT_MAX) {
    halt();
  }
  else if (in_edge != in_branch) {
    --find_count;

    if (in_weight < best_wt) {
      best_wt = in_weight;
      best_edge = in_edge;
    }

    report();
  }
  else if (state == FIND) {
    // Place the received message at the end of the incoming queue
    Comm::ghs_message msg;
    msg.type = Comm::REPORT;
    msg.second = in_weight;

    recv.emplace(in_edge, msg);
  }
  else if (in_weight > best_wt) {
    change_core();
  }
}

void GHSNode::change_core() {
  if (edge_states[in_branch] == BRANCH) {
    // Send Change-core on best-edge
    Comm::ghs_message msg;
    msg.type == Comm::CHANGE_CORE;

    send.emplace(best_edge, msg);
  }
  else {
    // Send Connect(LN) on best-edge
    Comm::ghs_message msg;
    msg.type = Comm::CONNECT;
    msg.first = level;

    send.emplace(best_edge, msg);

    edge_states[best_edge] = BRANCH;
  }
}

void GHSNode::halt() {
  // Clear every data structure
  std::queue<std::pair<int, Comm::ghs_message>>().swap(recv);
  std::queue<std::pair<int, Comm::ghs_message>>().swap(send);
  edges.clear();
  edge_states.clear();
  edge_weights.clear();

  // Set the halt flag
  halt_flag = 1;
}