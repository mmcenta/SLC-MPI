#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "graph/graph.h"
#include "algorithms/algorithms.h"
#include "io/read.h"

using namespace std;

void runtime_test(const char *filename, ostream& out) {
  double cost[3];

  // Add prefix
  stringstream ss;
  ss << "./test/data/" << filename;

  ifstream in(ss.str());
  if (!in.is_open()) {
    cerr << "Error while opening file " << filename << "." << endl;
    exit(1);
  }

  Graph g = read_edgelist(in);

  // Prim algorithm
  Prim pmst(g);
  pmst.solve();

  cost[0] = pmst.get_cost();

  // Kruskal algorithm
  Kruskal kmst(g);
  kmst.solve();

  cost[1] = kmst.get_cost();

  // Boruvka's algorithm
  Boruvka bmst(g);
  bmst.solve();

  cost[2] = bmst.get_cost();

  // Check the MST cost
  double sum_of_errors = 0.0;

  sum_of_errors += abs(cost[0]-cost[1]);
  sum_of_errors += abs(cost[1]-cost[2]);
  sum_of_errors += abs(cost[0]-cost[2]);

  if (sum_of_errors/(3*cost[2]) > 0.001) {
    cerr << "Wrong MST cost, relative error: " << sum_of_errors << "." << endl;
    exit(1);
  }

  // Print results to output
  out << g.get_num_vertices() << " " << g.get_num_edges() << " ";
  out << pmst.get_elapsed_time() << " ";
  out << kmst.get_elapsed_time() << " ";
  out << bmst.get_elapsed_time() << endl;
}

int main(int argc, char ** argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " output_file" << endl; 
    exit(1);
  }
  const char *out_file = argv[1];

  ofstream out(out_file);
  if (!out.is_open()) {
    cerr << "Error while opening file " << out_file << "." << endl;
    exit(1);
  }

  ifstream index("./test/data/index.txt");
  if (!index.is_open()) {
    cerr << "Error while opening file test/data/index.txt." << endl;
    exit(1);
  }

  // Read the number of tests
  int num_tests;
  string ignore;

  index >> num_tests;
  getline(index, ignore);

  // For each test, calculate results for each file
  while (num_tests--) {
    int num_files;
    string test_name, filename;

    // Read test name
    getline(index, test_name);
    
    // Read the number of files
    index >> num_files;

    // Write test name to output
    out << test_name << "\n" << num_files << endl;

    cout << "Starting test '" << test_name << "' with ";
    cout << num_files << " files..." << endl;

    while (num_files--) {
      index >> filename;
      runtime_test(filename.c_str(), out);
    }
    getline(index, ignore);

    cout << "Test finished." << endl;
  }

  return 0;
}