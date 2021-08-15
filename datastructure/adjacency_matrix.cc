#include <iostream>

constexpr size_t MaxVertexNum = 10;

struct GNode {
    int nv,ne;
    int G[MaxVertexNum][MaxVertexNum];
    int data[MaxVertexNum];
};

using MGraph = GNode*;
using Vertex = int;
MGraph create_graph(int vertex_num) {
    MGraph graph = new GNode;
    graph->nv = vertex_num;
    graph_ne = 0;
    for(Vertex v = 0;v < graph->nv;v ++) {
        for(Vertex w = 0;w < graph->nv;w ++) {
            graph->G[v][w] = 0;
        }
    }
    return graph;
}

struct ENode {
    Vertex v1,v2;
    size_t weight;
};

using Edge = ENode*;

void insert_edge(MGraph graph,Edge e) {
    graph->G[e->v1][e->v2] = e->weight;
    graph->G[e->v2][e->v1] = e->weight;
}

MGraph build_graph() {
    MGraph graph = create_graph(5);
    graph->ne = 5;
    if(graph->ne != 0) {
        Edge e = new ENode{};
        for(int i = 0;i < graph->ne;i ++) {
            // std::cin >> e->v1 >> e->v2 >> e->weight;
            insert_edge(graph,e);
        }
    }  
    return graph;
}



int main() {
    
    return 0;
}