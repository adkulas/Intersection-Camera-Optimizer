
#include <minisat/core/SolverTypes.h>
#include <minisat/core/Solver.h>
#include <regex>
#include <vector>
#include <iostream>
#include "cnf-sat-vc.hpp"

std::vector< std::pair<int,int> > parse(std::string s) {
    std::pair<int, int> edge;
    std::vector< std::pair<int,int> > result;
    
    // using regex
    try {
        std::regex re("-?[0-9]+"); //match consectuive numbers
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

int main() {
    char cmd;
    int vertices;
    std::string edges_input;
    VertexCover* v_cover = new VertexCover(0);

    while(std::cin >> cmd){    
        
        switch(cmd) {
            
            case 'V': case 'v':
                std::cin >> vertices;
                // std::cout << "V " << vertices << std::endl;    
                // Create a new graph
                if (vertices >= 0) {
                    delete v_cover;
                    v_cover = new VertexCover(vertices);
                } else {
                    std::cerr << "Error: Incorrect value for vertices entered" << std::endl;
                }
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;
            
            case 'E': case 'e':
                std::cin >> edges_input;
                // std::cout << "E " << edges_input << std::endl;
                v_cover->add_edges( parse(edges_input) );
                v_cover->find_minimum();

                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;

            // case 'S': case 's':
            //     std::cin >> start_vertex >> end_vertex;
            //     v_cover->print_shortest_path(start_vertex, end_vertex);
            //     std::cin.clear();
            //     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            //     break;

            default:
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Error: command not recognized" << std::endl;
        }
    }
    delete v_cover;
    return 0;
}