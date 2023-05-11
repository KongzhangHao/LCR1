#include "graph.h"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>

using std::ifstream;
using std::string;
using std::istringstream;



Option::Option(){
    this->members = set<label_t>();
}
Option::Option(label_t label){
    this->members = set<label_t>();
    this->members.insert(label);
}
void Option::addMember(label_t label){
    this->members.insert(label);
}
bool Option::operator<(const Option& other) const{
    // an option is smaller if it has less members
    // for example, {1,2,3} < {1,2,3,4}
    // if they have the same number of members, then compare the members
    // for example, {1,2,3} < {1,2,4}
    if(this->members.size() == other.members.size()){
        return std::lexicographical_compare(
            this->members.begin(), this->members.end(),
            other.members.begin(), other.members.end());
    }
    return this->members.size() < other.members.size();
 }






Edge::Edge(uint32_t from, uint32_t to){
    this->from = from;
    this->to = to;
    this->labelOpts = set<Option> ();
}
void Edge::addLabelOption(Option option){
    // copy option
    Option newOption = Option();
    for(auto label : option.members){
        newOption.addMember(label);
    }

    this->labelOpts.insert(newOption);
}

uint32_t Edge::getEdgeSize() {
    uint32_t size = sizeof(from) + sizeof(to);
    size += labelOpts.size() * sizeof(uint32_t);
    return size;
}









Graph::Graph(){
    vertices = unordered_set<uint32_t>();
    inEdges = unordered_map<uint32_t,vector<Edge>>();
    outEdges = unordered_map<uint32_t,vector<Edge>>();
}

void Graph::addVertex(uint32_t vertex){
    vertices.insert(vertex);
}

void Graph::addEdge(uint32_t from, uint32_t to, uint32_t label){
    vertices.insert(from);
    vertices.insert(to);
    Edge edge(from,to);
    Option opt(label);
    edge.addLabelOption(opt);
    inEdges[to].push_back(edge);
    outEdges[from].push_back(edge);
}

void Graph::addEdge(uint32_t from, uint32_t to, set<Option> labelOptions){
    vertices.insert(from);
    vertices.insert(to);

    for(auto &edge : outEdges[from]){
        // if edge exist, add label options to it
        if(edge.to == to){
            for(auto option : labelOptions){
                edge.addLabelOption(option);
            }
            return;
        }
    }

    // otherwise, create a new edge
    Edge edge(from,to);
    for(auto option : labelOptions){
        edge.addLabelOption(option);
    }
    inEdges[to].push_back(edge);
    outEdges[from].push_back(edge);
}



unordered_set<uint32_t> Graph::findMIS(){
    // get a sorted list of vertices where the first vertex has the least in degree + out degree, if same degree, then the one with smaller vertex id
    vector<uint32_t> sortedVertices = vector<uint32_t>();
    for(auto vertex : vertices){
        sortedVertices.push_back(vertex);
    }
    std::sort(sortedVertices.begin(), sortedVertices.end(), [this](uint32_t a, uint32_t b){
        uint32_t aDegree = inEdges[a].size() + outEdges[a].size();
        uint32_t bDegree = inEdges[b].size() + outEdges[b].size();
        if(aDegree == bDegree){
            return a < b;
        }
        return aDegree < bDegree;
    });

    unordered_set<uint32_t> mis = unordered_set<uint32_t>();
    unordered_set<uint32_t> nonMis = unordered_set<uint32_t>();
    for(auto vertex : sortedVertices){
        if(nonMis.find(vertex) == nonMis.end()){
            mis.insert(vertex);
            for(auto edge : inEdges[vertex]){
                nonMis.insert(edge.from);
            }
            for(auto edge : outEdges[vertex]){
                nonMis.insert(edge.to);
            }
        }
    }
    return mis;
}


void Graph::printGraphSize(){
    uint32_t size = 0;
    uint32_t edgeCount = 0;
    for(auto vertex : vertices){
        for(auto edge : inEdges[vertex]){
            size += edge.getEdgeSize();
            edgeCount++;
        }
    }
    //std::cout << "Graph with " << vertices.size() << "vertices and "<< edgeCount <<" edges is of Size: " << size << std::endl;
    std::cout << vertices.size() << " "<< edgeCount <<" " << size  << std::endl;

}

void Graph::printGraph(){
    std::cout << "Graph: " << std::endl;
    for(auto vertex : vertices){
        std::cout << "Vertex: " << vertex << std::endl;
        std::cout << "In Edges: " << std::endl;
        for(auto edge : inEdges[vertex]){
            std::cout << "\tFrom: " << edge.from << " To: " << edge.to << " Labels: ";
            
            for(auto label : edge.labelOpts){
                std::cout<<"{";
                for (auto member : label.members){
                    std::cout << member << ",";
                }
                std::cout <<"}, ";
            }
            std::cout << std::endl;
        }
        std::cout << "Out Edges: " << std::endl;
        for(auto edge : outEdges[vertex]){
            std::cout << "\tFrom: " << edge.from << " To: " << edge.to << " Labels: ";
            for(auto label : edge.labelOpts){
                std::cout<<"{";
                for (auto member : label.members){
                    std::cout << member << ",";
                }
                std::cout <<"}, ";
            }
            std::cout << std::endl;
        }
    }
}


void loadGraph(Graph &graph, const char *filename){
    ifstream infile(filename);
    uint32_t from;
    uint32_t to;
    label_t label;

    while(infile >> from >> to >> label){
        graph.addVertex(from);
        graph.addVertex(to);
        graph.addEdge(from,to,label);
    }
}

void misDecomposition(Graph &graph, vector<Graph> &G, vector<unordered_set<uint32_t>> &I){
    G.push_back(graph);
    I.push_back(graph.findMIS());
    int i = 0;
    
    Graph currentGraph = G[i];
    Graph newGraph = Graph();
    // while the current graph has more than 1 vertex
    while(!currentGraph.vertices.empty()){
        // print current graph vertice size
        //std::cout << "Current Graph Vertices Size: " << currentGraph.vertices.size() << std::endl;
        // for each vertex in the current graph
        for(auto vertex : currentGraph.vertices){
            // add all the edge that both vertices are not in MIS to the new graph
            if(I[i].find(vertex) == I[i].end()){
                newGraph.addVertex(vertex);
                for(auto edge : currentGraph.outEdges[vertex]){
                    if(I[i].find(edge.to) == I[i].end()){
                        newGraph.addVertex(edge.to);
                        newGraph.addEdge(edge.from,edge.to,edge.labelOpts);
                    }
                }
            }else{
                // if the vertex is in the MIS, we add all in edges X out edges to the new graph
                for(auto edgeIn : currentGraph.inEdges[vertex]){
                    for(auto edgeOut : currentGraph.outEdges[vertex]){
                        // if edgein.from == edgeout.to, then we skip
                        if(edgeIn.from == edgeOut.to){
                            continue;
                        }
                        // get all combinations of in edge options and out edge options
                        set<Option> newLabelOpts = set<Option>(); 
                        for(auto inLabel : edgeIn.labelOpts){
                            for(auto outLabel : edgeOut.labelOpts){
                                Option newOption = Option();
                                for(auto member : inLabel.members){
                                    newOption.addMember(member);
                                }
                                for(auto member : outLabel.members){
                                    newOption.addMember(member);
                                }
                                newLabelOpts.insert(newOption);
                            }
                        }

                        // add the edge to the new graph
                        newGraph.addVertex(edgeIn.from);
                        newGraph.addVertex(edgeOut.to);
                        newGraph.addEdge(edgeIn.from,edgeOut.to,newLabelOpts);
                    }
                }
            }
        }
        // save and increment
        G.push_back(newGraph);
        I.push_back(newGraph.findMIS());
        currentGraph = newGraph;
        newGraph = Graph();
        i++;            
    }
}
