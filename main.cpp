
#include <minisat/core/SolverTypes.h>
#include <minisat/core/Solver.h>
#include <regex>
#include <vector>
#include <iostream>
#include "cnf-sat-vc.hpp"
#include "approx_vc.hpp"


std::vector< std::pair<int,int> > parse(std::string s) {
    std::pair<int, int> edge;
    std::vector< std::pair<int,int> > result;
    
    // using regex
    try {
        std::regex re("-?[0-9]+"); //match consectuive numbers, matches lazily
        std::sregex_iterator next(s.begin(), s.end(), re);
        std::sregex_iterator end;
        while (next != end) {
            std::smatch match1;
            std::smatch match2;
            
            match1 = *next;
            next++;
            // iterate to next match
            if (next != end) {
                match2 = *next;
                edge.first = std::stoi(match1.str());
                edge.second = std::stoi(match2.str());
                result.push_back(edge);
                next++;
            }
        } 
    } 
    catch (std::regex_error& e) {
        result.clear();
    }

    return result;
}


int cnf_sat_vc(int vertices, std::vector< std::pair<int,int> > edges) {
    VertexCover v_cover = VertexCover(vertices, edges);
    // v_cover.add_edges(edges);
    v_cover.lin_search_vcover();

    return 0;
}

int IO_handler() {
    char cmd;
    int vertices;
    std::string edges_input;
    std::vector< std::pair<int,int> > parsed_edges;

    while(std::cin >> cmd){    
        
        switch(cmd) {
            
            case 'V': case 'v':
                std::cin >> vertices;
                // std::cout << "V " << vertices << std::endl;    
                if (vertices < 0) {
                    std::cerr << "Error: Incorrect value for vertices entered" << std::endl;
                    vertices = 0;
                }
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;
            
            case 'E': case 'e':
                std::cin >> edges_input;
                // std::cout << "E " << edges_input << std::endl;
                parsed_edges = parse(edges_input);
                cnf_sat_vc(vertices, parsed_edges);
                approx_vc_1(vertices, parsed_edges);
                approx_vc_2(vertices, parsed_edges);

                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;

            default:
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Error: command not recognized" << std::endl;
        }
    }
    return 0;
}


int main() {
    IO_handler();
    return 0;
}