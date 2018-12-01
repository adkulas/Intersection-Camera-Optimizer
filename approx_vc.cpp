#include <iostream>
#include <vector>
#include <list>
#include <climits>
#include "approx_vc.hpp"

bool check_valid_input(int v, std::vector< std::pair<int,int> > edges) {
    for ( auto& e : edges) {
        if (v <= e.first || v <= e.second || e.first < 0 || e.second < 0) {
            std::cerr << "Error: Attempted to add edge to vertex that does not exist"
                      << std::endl;
            return false;
        }
        // if (e.first == e.second) {
        //     std::cerr << "Error:  Cannot add edge from vertex to iteself"
        //               << std::endl;
        //     return false;
        // }
    }
    return true;
}


std::vector< std::vector<int> > create_graph_adj_list(int v, std::vector< std::pair<int,int> > edges) {
    std::vector< std::vector<int> > graph(v);
    for ( auto& e : edges) {
            std::cout << e.first << " " << e.second << std::endl;
            graph[e.first].push_back(e.second);
            graph[e.second].push_back(e.first);
        }
    return graph;
    
}

void approx_vc_1(int v, std::vector< std::pair<int,int> > edges) {
    if (!check_valid_input(v, edges)) {
        return;
    }

    std::vector< std::vector<int> > graph;
    graph = create_graph_adj_list(v, edges);
    std::cout << "APPROX-VC-1: " << std::endl;

    return;
}

void approx_vc_2(int v, std::vector< std::pair<int,int> > edges) {
    if (!check_valid_input(v, edges)) {
        return;
    }

    std::vector< std::vector<int> > graph;
    graph = create_graph_adj_list(v, edges);
    std::cout << "APPROX-VC-2: " << std::endl;

    return;
}


