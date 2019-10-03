#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/graph/graphml.hpp>

typedef boost::adjacency

void show_usage();
void show_help();

int main(int argc, char ** argv) {
    bool boost_benchmark = false;

    if (argc == 2) {
        if(strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "--boost"))
            boost_benchmark = true;
        else if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help"))
            show_help();
        else
            show_usage();
    }
    else if(argc > 2) {
        show_usage();
    }

    // iterate over the internet zoo files
    boost::filesystem::path datapath("./data/internet_zoo");
    if (boost::filesystem::exists(datapath)) {
        fprintf(stderr, "Data directory not found, check that /test/data/internet_zoo exists");
        exit(1);
    }

    for(boost::filesystem::directory_entry& it :
        boost::filesystem::directory_iterator(datapath)) {
        // check if the extension is correct
        if(it.path().extension().string() == ".graphml") { 
            boost::adjacency_list< boost::vecS, boost::vecS, boost::directedS, 
        }
    }
    


    
    return 0;
}

void test_component(boost::filesystem::path graphml_path) {
    std::ifstream graphml_file;
    if (graphml_file.open(graphml_path.string()), std::ios::in) {
        boost::mutable_graph 
        boost::read_graphml(graphml_file, )
    }
}

void show_usage() { }
void show_help() { }