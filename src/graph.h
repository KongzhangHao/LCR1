#ifndef BE3FE588_EE77_4CBC_9100_B98182A682F9
#define BE3FE588_EE77_4CBC_9100_B98182A682F9

#include <vector>
#include <cstdint>
#include <set>
#include <unordered_set>
#include <unordered_map>

using std::vector;
using std::set;
using std::unordered_set;
using std::unordered_map;

#define label_t uint32_t

class Option{
    public:
    set<label_t> members;

    Option(label_t label);
    Option();
    void addMember(label_t label);
    
    bool operator<(const Option& other) const;
};

/**
an edge is composed of a from vertex, a to vertex, and a set of label options
Example:
edge from 1 to 2 with label options {1,2,3} and {4,5} is like:
edge{
    from: 1,
    to: 2,
    labelOpts: {
        {1,2,3},
        {4,5}
    }
}
indicates that we can go through 1->2 with either 1,2,3 or 4,5

*/

class Edge{
    public:
    uint32_t from;
    uint32_t to;
    set<Option> labelOpts;    

    Edge(uint32_t from, uint32_t to);
    void addLabelOption(Option option);
    uint32_t getEdgeSize();
};


class Graph {
    public:
    unordered_set<uint32_t> vertices;
    unordered_map<uint32_t,vector<Edge>> inEdges;
    unordered_map<uint32_t,vector<Edge>> outEdges;
    

    Graph();
    void addVertex(uint32_t vertex);
    void addEdge(uint32_t from, uint32_t to, label_t label);
    void addEdge(uint32_t from, uint32_t to, set<Option> options);
    
    unordered_set<uint32_t> findMIS();
    
    void printGraph();
    void printGraphSize();
};

void loadGraph(Graph& graph, const char* filename);
void misDecomposition(Graph& graph, vector<Graph> &G, vector<unordered_set<uint32_t>> &I);

#endif /* BE3FE588_EE77_4CBC_9100_B98182A682F9 */
