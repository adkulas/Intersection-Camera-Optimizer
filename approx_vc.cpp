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
        if (e.first == e.second) {
            std::cerr << "Error:  Cannot add edge from vertex to itself"
                      << std::endl;
            return false;
        }
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


std::string approx_vc_1(int v, std::vector< std::pair<int,int> > edges) {
    if (!check_valid_input(v, edges)) {
        return "";
    }

    if (edges.empty()) {
        return "";
    }

    std::vector<int> v_cover;
    std::vector< std::vector<int> > graph = create_graph_adj_matrix(v, edges);

    while(true) {
        // pick highest degree vertex
        int vertex = -1;
        int max_degree = 0;
        for (unsigned int r=0; r < graph.size(); r++) {
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
        for (unsigned int i=0; i < graph.size(); i++) {
            graph[i][vertex] = 0; // set column to 0
            graph[vertex][i] = 0; // set row to 0
        }

        // add vertex to cover
        v_cover.push_back(vertex);
        
        // debug messages
        // std::clog << "Adding vertex: " << vertex << std::endl;
        // std::clog << "Current Graph:" << std::endl;
        // for (auto& r : graph) {
        //     for (auto& c : r) {
        //         std::clog << c << " ";
        //     }
        //     std::clog << std::endl;
        // }
    }
    std::sort(v_cover.begin(), v_cover.end());

    return print_vector(v_cover);
}


std::string approx_vc_2(int v, std::vector< std::pair<int,int> > edges) {
    if (!check_valid_input(v, edges)) {
        return "";
    }

    if (edges.empty()) {
        return "";
    }

    std::vector<int> v_cover;
    std::vector< std::vector<int> > graph = create_graph_adj_matrix(v, edges);

    while(true) {
        
        // find an edge E = <v1, v2>
        int v1;
        int v2;
        bool found_flag = false;
        for (int r=0; r < v; r++) {
            for (int c=0; c < v; c++) {
                if (graph[r][c] == 1) {
                    v1 = r;
                    v2 = c;
                    found_flag = true;
                    break;
                } 
            }
            if (found_flag) {break;} // exit first for loop
        }
    
        // exit while loop if no edges remain
        if (!found_flag) {break;}

        // add vertices to vertex cover
        v_cover.push_back(v1);
        v_cover.push_back(v2);

        // remove edges incident to vertices
        for (unsigned int i=0; i < graph.size(); i++) {
            graph[v1][i] = 0; // set row to 0
            graph[v2][i] = 0; // set row to 0       
            graph[i][v1] = 0; // set column to 0
            graph[i][v2] = 0; // set column to 0     

        }

        // debug messages
        // std::clog << "Adding vertex: " << v1 << " and " << v2 << std::endl;
        // std::clog << "Current Graph:" << std::endl;
        // for (auto& r : graph) {
        //     for (auto& c : r) {
        //         std::clog << c << " ";
        //     }
        //     std::clog << std::endl;
        // }
    }

    std::sort(v_cover.begin(), v_cover.end());

    return print_vector(v_cover);
}


