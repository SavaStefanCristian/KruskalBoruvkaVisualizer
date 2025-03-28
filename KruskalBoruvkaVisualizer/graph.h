#ifndef GRAPH_H
#define GRAPH_H
#pragma once

#include "node.h"
#include <vector>
#include <unordered_map>
#include <string>


class Graph
{
public:
    Graph();
    ~Graph();
    void AddNode(QPoint pos);
    void AddEdge(Node* first, Node* second, int cost);
    void RemoveEdge(Node* first, Node* second);
    const std::vector<Node*>& GetNodes() {return m_nodes;}
    const std::unordered_map<Node*,std::unordered_map<Node*,int>>& GetEdges() {return m_edges;}
    std::unordered_map<Node*,std::unordered_map<Node*,int>> Boruvka();
    std::unordered_map<Node*,std::unordered_map<Node*,int>> Kruskal();

private:
    void Union(std::unordered_map<Node*, Node*>& parent, std::unordered_map<Node*, int>& rank, Node* node1, Node* node2);
    Node* FindParent(std::unordered_map<Node*, Node*>& parent, Node* node);

private:
    std::vector<Node*> m_nodes;
    std::unordered_map<Node*,std::unordered_map<Node*,int>> m_edges;
    std::string name;
};

#endif // GRAPH_H
