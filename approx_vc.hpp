#pragma once

#include <iostream>
#include <vector>
#include "cnf-sat-vc.hpp"

// helper functions
std::vector< std::vector<int> > create_graph_adj_list(int v, std::vector< std::pair<int,int> > edges);
std::vector< std::vector<int> > create_graph_adj_matrix(int v, std::vector< std::pair<int,int> > edges);
bool check_valid_input(int v, std::vector< std::pair<int,int> > edges);

// approximation vertex cover algorithm 1
void approx_vc_1(int v, std::vector< std::pair<int,int> > edges);

// approximation vertex cover algorithm 2
void approx_vc_2(int v, std::vector< std::pair<int,int> > edges);
