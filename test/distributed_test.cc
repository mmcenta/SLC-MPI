#include <iostream>
#include <fstream>

#include "graph/graph.h"
#include "algorithms/kruskal.h"
#include "distributed/distributed_prim.h"
#include "distributed/distributed_boruvka.h"
#include "io/read.h"

using namespace std;

int main(int argc, char ** argv) {
    int num_proc, rank;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    ifstream in("./data/er1.txt");
    if (!in.is_open()) {
        cerr << "Error while opening file" << endl;
        exit(1);
    }

    Graph g = read_ashiip(in);


    if (rank == 0) {
        cout << "Sequential Kruskal: " << endl;
    
        Kruskal kmst(g);
        kmst.solve();

        cout << "MST cost = " << kmst.get_cost() << ", ";
        cout << "time elapsed: " << kmst.get_elapsed_time() << endl;
        cout << "--------------------------------------------------------------------------------" << endl;
    }

    if (rank == 0) cout << "Distributed Prim: " << endl;
    
    DistributedPrim pmst(g);
    pmst.solve();

    if (rank == 0) {
        cout << "MST cost = " << pmst.get_cost() << ", ";
        cout << "time elapsed: " << pmst.get_elapsed_time() << endl;
        cout << "--------------------------------------------------------------------------------" << endl;
    }

    if (rank == 0) cout << "Distributed Boruvka: " << endl;
    
    DistributedBoruvka bmst(g);
    bmst.solve();

    if (rank == 0) {
        cout << "MST cost = " << bmst.get_cost() << ", ";
        cout << "time elapsed: " << bmst.get_elapsed_time() << endl;
        cout << "--------------------------------------------------------------------------------" << endl;
    }

    MPI_Finalize();

    return 0;
}