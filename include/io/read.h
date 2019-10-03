#ifndef READ_H
#define READ_H

#include <istream>

#include "graph/graph.h"


Graph read_ashiip(std::istream& in);
Graph read_edgelist(std::istream& in);

#endif