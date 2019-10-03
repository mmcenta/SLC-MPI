#ifndef GHS_H
#define GHS_H

#include <queue>
#include <unordered_map>

#include "graph/graph.h"
#include "ghs_node.h"


class GHS {
    private:
        GHSNode local_nodes;
        std::unordered_map<Edge, std::queue<>> message_map;
};


#endif // GHS_H