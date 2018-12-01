#include <iostream>
#include <vector>
#include <list>
#include <climits>
#include <algorithm>
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
            graph[e.first].push_back(e.second);
            graph[e.second].push_back(e.first);
        }
    return graph;
}


std::vector< std::vector<int> > create_graph_adj_matrix(int v, std::vector< std::pair<int,int> > edges) {
    
    // initialize v x v matrix
    std::vector< std::vector<int> > graph(v);
    for ( int i = 0 ; i < v ; i++ )
        graph[i].resize(v);

    // add edges between vertices
    for ( auto& e : edges) {
            graph[e.first][e.second] = 1;
            graph[e.second][e.first] = 1;
        }
    return graph;
}

void approx_vc_1(int v, std::vector< std::pair<int,int> > edges) {
    if (!check_valid_input(v, edges)) {
        return;
    }

    std::vector<int> v_cover;
    std::vector< std::vector<int> > graph = create_graph_adj_matrix(v, edges);

    for (auto r : graph) {
        for (auto c : r) {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }
    while(true) {
        // pick highest degree vertex
        int vertex = -1;
        int max_degree = 0;
        for (int r=0; r < graph.size(); r++) {
            int v_deg = 0;
            for (auto& n : graph[r])
                v_deg += n;
             
            if (v_deg > max_degree) {
                vertex = r;
                max_degree = v_deg;
            }
        }
        
        // break if no edges remain (ever vertex has degree of 0)
        if (vertex < 0) {
            break;
        }

        // remove edges incident to vertex
        for (int i=0; i < graph.size(); i++) { // reset column to 0
            graph[i][vertex] = 0;
            graph[vertex][i] = 0;
        }

        // add vertex to cover
        v_cover.push_back(vertex);
        std::clog << "Adding vertex: " << vertex << std::endl;
        std::clog << "Current Graph:" << std::endl;
        for (auto& r : graph) {
            for (auto& c : r) {
                std::clog << c << " ";
            }
            std::clog << std::endl;
        }
    }
    std::sort(v_cover.begin(), v_cover.end());

    std::cout << "APPROX-VC-1: ";
    for (auto& v : v_cover) {
        std::cout << v << " ";
    } 
    std::cout << std::endl;

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


