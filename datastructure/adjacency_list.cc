#include <iostream>

constexpr size_t MaxVertexNum = 10;

struct ListNode {
    int adjv; // 邻接点下标
    int weight;
    ListNode *next;
};

struct VNode {
    ListNode *edge;
    int data;
};

using AjdList = VNode[MaxVertexNum];

struct GNode {
    int ne // 边数
    int nv; // 顶点数
    AdjList G; // 邻接表
};

using Vertex = int;
using LGraph = GNode*;

LGraph create_graph(int vertex_num) {
    Vertex v,w;
    LGraph graph = new GNode {};
    graph->nv = vertex_num;
    graph->ne = 0;

    for(v = 0;v < graph->nv;v ++) {
        graph->G[v].edge = nullptr;
    }
    return graph;
}   

struct Edge {
    Vertex v1;
    Vertex v2;
    int weight;
};

void insert_edge(LGraph graph,Edge e) {
    ListNode *node = new ListNode {};
    node->adjv = e->v2;
    node->weight = e->weight;

    node->next = graph->G[e->v1].edge;
    graph->G[e->v1].edge = node;

    {
        ListNode *node = new ListNode {};
        node->adjv = e->v1;
        node->weight = e->weight;

        node->next = graph->G[e->v2].edge;
        graph->G[e->v2].edge = node;
    }
}

using LGraph = GNode*;

int main() {
    
    return 0;
}