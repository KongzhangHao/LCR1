// test load graph with data from ./data/graph.binedge
#include "graph.h"
#include <iostream>
#include <vector>


int main(int argc, char** argv){



    Graph graph = Graph();
    loadGraph(graph, "/home/summer/Desktop/LCR2/data/data.txt");
    std::vector<Graph> G;
    std::vector<std::unordered_set<uint32_t>> I;
    misDecomposition(graph, G, I);
    for(auto graph : G){
        graph.printGraphSize();
    }

    // for(auto mis : I){
    //     std::cout << "MIS: ";
    //     for(auto vertex : mis){
    //         std::cout << vertex << " ";
    //     }
    //     std::cout << std::endl;
    // }
    return 0;
}