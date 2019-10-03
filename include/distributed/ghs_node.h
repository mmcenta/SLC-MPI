#ifndef GHS_NODE_H
#define GHS_NODE_H

#include <map>
#include <queue>
#include <utility> // for std::pair

#include "ghs_comm.h"


enum NodeState {
  SLEEPING = 0,
  FIND,
  FOUND
};

enum EdgeState {
  BASIC = 0,
  BRANCH,
  REJECTED
};

class GHSNode {
  private:
    int id; // this node's id
    int halt_flag;

    NodeState state; // SN
    int fragment;    // FN (local fragment identity)
    int level;       // LN
    int find_count;  // find-count

    int best_edge;   // best-edge
    double best_wt;  // best-wt

    int test_edge;   // test-edge
    double test_wt;  // test-wt
    
    int in_branch;   // in-branch

    // Queues to hold incoming and outbound messages
    std::queue<std::pair<int, Comm::ghs_message>> recv;
    std::queue<std::pair<int, Comm::ghs_message>> send;

    std::vector<int> edges;
    std::unordered_map<int, double> edge_weights;
    std::unordered_map<int, EdgeState> edge_states;

  public:
    GHSNode(std::vector<Edge> adjacent_edges);

    void find_best_basic(int& best_basic_node, double& best_basic_edge);

   /**  \brief Procedure to wake up an sleeping edge.
    *   It's executed either to start the GHS algorithm or when a sleeping node
    *   receives a message.
    */
    void wakeup(); 

   /** \brief Procedure to respond a Connect(L) message that was received on edge
    *   in_edge.
    *
    *  \param in_edge The edge this message was received on.
    *  
    *  \param in_level The incoming level L attached to the message.
    */
    void respond_connect(int in_level, int in_edge);

   /** \brief Procedure to respond a Initiate(L, F, S) message that was received on
    *   edge in_edge.
    * 
    *  \param in_edge The edge this message was received on.
    *  
    *  \param in_level The incoming level L attached to the message.
    * 
    *  \param in_fragment The incoming fragment identifier F attached to the message.
    * 
    *  \param in_state The incoming state S attached to the message. 
    */
    void respond_initiate(int in_edge, int in_level, double in_fragment, EdgeState in_state);

    void test();

    void respond_test(int in_edge, int in_level, double in_fragment);

    void respond_accept(int in_edge);

    void respond_reject(int in_edge); // harsh

    void report();

    void respond_report(int in_edge, double in_weight);

    void change_core();

    void halt();

    void receive_message(Comm::ghs_message msg, Edge in_edge);
    void send_message(Comm::ghs_message& msg, Edge& send_edge);
};

#endif // GHS_NODE_H