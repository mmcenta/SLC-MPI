#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "graph/graph.h"
#include "distributed/distributed_prim.h"
#include "distributed/distributed_boruvka.h"
#include "io/read.h"

using namespace std;


void runtime_test(const char *filename, ostream& out, const int rank) {
  double cost[2];

  // Add prefix
  stringstream ss;
  ss << "./test/data/" << filename;

  ifstream in(ss.str());
  if (!in.is_open()) {
    cerr << "[proc " << rank << "] Error while opening file " << filename << "." << endl;
    exit(1);
  }

  Graph g = read_edgelist(in);

  // Distributed Prim algorithm
  DistributedPrim pmst(g);
  pmst.solve();

  cost[0] = pmst.get_cost();

  // Boruvka's algorithm
  DistributedBoruvka bmst(g);
  bmst.solve();

  cost[1] = bmst.get_cost();

  // Check the MST cost
  if (rank == 0 && abs(cost[0]-cost[1])/(cost[1]) > 0.001) {
    cerr << "[proc " << rank << "] Wrong MST cost, relative error: " << abs(cost[0]-cost[1])/(cost[2]) << "." << endl;
    exit(1);
  }

  // Print results to output
  if (rank == 0) {
    out << g.get_num_vertices() << " " << g.get_num_edges() << " ";
    out << pmst.get_elapsed_time() << " ";
    out << bmst.get_elapsed_time() << endl;
  }
}

int main(int argc, char ** argv) {
  int num_proc, rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc != 2 && rank == 0) {
    cerr << "Usage: " << argv[0] << " output_file" << endl; 
    exit(1);
  }
  const char *out_file = argv[1];

  ofstream out;
  if (rank == 0) {
    out.open(out_file);
    if (!out.is_open()) {
      cerr << "Error while opening output file " << out_file << "." << endl;
      exit(1);
    }
  }

  // Print the number of processes to the output file
  if (rank == 0) out << "Number of processes: " << num_proc << endl;

  ifstream index("./test/data/index.txt");
  if (!index.is_open()) {
    cerr << "[proc " << rank << "] Error while opening file test/data/index.txt." << endl;
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
    if (rank == 0) {
      out << test_name << "\n" << num_files << endl;

      cout << "Starting test '" << test_name << "' with ";
      cout << num_files << " files..." << endl;
    }

    while (num_files--) {
      index >> filename;
      runtime_test(filename.c_str(), out, rank);
    }
    getline(index, ignore);

    if (rank == 0) cout << "Test finished." << endl;
  }

  MPI_Finalize();

  return 0;
}