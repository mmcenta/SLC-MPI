#include <iostream>
#include <fstream>

#include "graph/graph.h"
#include "algorithms/algorithms.h"
#include "io/read.h"

using namespace std;

int main(int argc, char ** argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " ashiip_file" << endl; 
    exit(1);
  }
  const char *in_file = argv[1];

  fstream in(in_file);
  if (!in.is_open()) {
    cerr << "Error while opening file " << in_file << "." << endl;
    exit(1);
  }

  Graph g = read_ashiip(in);

  cout << "Prim: " << endl;
    
  Prim pmst(g);
  pmst.solve();

  cout << "MST cost = " << pmst.get_cost() << ", ";
  cout << "time elapsed: " << pmst.get_elapsed_time() << endl;

  cout << "--------------------------------------------------------------------------------" << endl;

  cout << "Kruskal: " << endl;
    
  Kruskal kmst(g);
  kmst.solve();

  cout << "MST cost = " << kmst.get_cost() << ", ";
  cout << "time elapsed: " << kmst.get_elapsed_time() << endl;
    
  cout << "--------------------------------------------------------------------------------" << endl;
    
  cout << "Boruvka: " << endl;
    
  Boruvka bmst(g);
  bmst.solve();

  cout << "MST cost = " << bmst.get_cost() << ", ";
  cout << "time elapsed: " << bmst.get_elapsed_time() << endl;

  return 0;
}
