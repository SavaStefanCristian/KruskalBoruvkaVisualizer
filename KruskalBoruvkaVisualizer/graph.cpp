#include "graph.h"
#include "unordered_set"
#include <QDebug>

Graph::Graph() {}

Graph::~Graph()
{
    for(size_t i = 0; i < m_nodes.size(); i++) {
        if(m_nodes[i]) delete m_nodes[i];
    }
}

void Graph::AddNode(QPoint pos)
{
    Node* n = new Node((int)m_nodes.size(), pos);
    m_nodes.push_back(n);
}

void Graph::AddEdge(Node* first, Node* second, int cost) {
    m_edges[first][second] = cost;
    m_edges[second][first] = cost;
}

void Graph::RemoveEdge(Node *first, Node *second)
{
    m_edges[first].erase(second);
    if(m_edges[first].size()==0) m_edges.erase(first);
    m_edges[second].erase(first);
    if(m_edges[second].size()==0) m_edges.erase(second);
}

std::unordered_map<Node *, std::unordered_map<Node *, int> > Graph::Boruvka()
{
    std::unordered_map<Node*,std::unordered_map<Node*,int>> sortedEdges;
    std::vector<std::pair<std::unordered_set<Node*>,bool>> components{m_nodes.size()};
    for(size_t i = 0 ; i < components.size(); ++i) {
        components[i].first.insert(m_nodes[i]);
        components[i].second = true;
    }
    while(components.size()>1) {
        for(size_t i = 0 ; i < components.size(); ++i) {
            if(components[i].second==false) continue;
            int minCost = INT_MAX;
            Node* minCostNode1 = nullptr;
            Node* minCostNode2 = nullptr;
            for(Node* node : components[i].first) {
                if(m_edges.find(node)==m_edges.end()) continue;
                for(auto [neighbour, cost] : m_edges[node]) {
                    if(components[i].first.find(neighbour) != components[i].first.end()) continue;
                    if(cost < minCost) {
                        minCost = cost;
                        minCostNode1 = node;
                        minCostNode2 = neighbour;
                    }
                }
            }
            if(minCostNode1==nullptr || minCostNode2 == nullptr) {
                qDebug() << "Not connected graph!\n";
                return std::unordered_map<Node *, std::unordered_map<Node *, int> >{};
            }
            sortedEdges[minCostNode1][minCostNode2] = minCost;
            sortedEdges[minCostNode2][minCostNode1] = minCost;
            size_t j = 0;
            for(; j < components.size(); ++j) {
                if(components[j].second == false) continue;
                if(components[j].first.find(minCostNode2)!=components[j].first.end()) break;
            }
            for(Node* node : components[j].first) {
                components[i].first.insert(node);
            }
            components[j].second = false;
        }
        std::vector<std::pair<std::unordered_set<Node*>,bool>> newComponents;
        for(size_t i =0; i < components.size(); ++i) {
            if(components[i].second==true) newComponents.emplace_back(components[i]);
        }
        components = newComponents;
    }
    return sortedEdges;
}





Node* Graph::FindParent(std::unordered_map<Node*, Node*>& parent, Node* node) {
    Node* current = node;
    while (parent[current] != current) {
        current = parent[current];
    }
    Node* root = current;
    current = node;
    while (parent[current] != current) {
        Node* next = parent[current];
        parent[current] = root;
        current = next;
    }
    return root;
}


void Graph::Union(std::unordered_map<Node*, Node*>& parent, std::unordered_map<Node*, int>& rank, Node* node1, Node* node2) {
    Node* root1 = FindParent(parent, node1);
    Node* root2 = FindParent(parent, node2);

    if (root1 != root2) {
        if (rank[root1] > rank[root2]) {
            parent[root2] = root1;
        } else if (rank[root1] < rank[root2]) {
            parent[root1] = root2;
        } else {
            parent[root2] = root1;
            rank[root1]++;
        }
    }
}


std::unordered_map<Node*, std::unordered_map<Node*, int>> Graph::Kruskal() {

    std::vector<std::pair<std::pair<Node*, Node*>, int>> edges;
    for (auto& [first, connections] : m_edges) {
        for(auto& [second, cost] : connections) {
            if(first < second) {
                edges.push_back({{first,second},cost});
            }
        }
    }

    auto edgeComparator = [](const auto& a, const auto& b) {
        return a.second < b.second;
    };
    std::sort(edges.begin(), edges.end(), edgeComparator);

    std::unordered_map<Node*, Node*> parent;
    std::unordered_map<Node*, int> rank;
    for (Node* node : m_nodes) {
        parent[node] = node;
        rank[node] = 0;
    }

    std::unordered_map<Node*, std::unordered_map<Node*, int>> mst;
    int mstEdgeCount = 0;

    for (const auto& edge : edges) {
        Node* const& first = edge.first.first;
        Node* const & second = edge.first.second;
        const int& cost = edge.second;

        if (FindParent(parent, first) != FindParent(parent, second)) {
            mst[first][second] = cost;
            mst[second][first] = cost;
            Union(parent, rank, first, second);
            mstEdgeCount++;
        }
    }
    if(mstEdgeCount != m_nodes.size() - 1) {
        qDebug() << "Not connected graph!\n";
        return std::unordered_map<Node *, std::unordered_map<Node *, int> >{};
    }
    return mst;
}

