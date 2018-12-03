
#include <minisat/core/SolverTypes.h>
#include <minisat/core/Solver.h>
#include <regex>
#include <vector>
#include <list>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
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
    v_cover.lin_search_vcover();

    return 0;
}


void* IO_handler(void* args) {
    char cmd;
    int vertices = 0;
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
    return NULL;
}

struct job {
  int vertices;
  std::string edges;
};

pthread_mutex_t job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
std::list<job*> job_queue;  // stores a list of pointers to jobs (structs containing information)
std::list<std::string> result_queue;

void* test_producer(void* args) {
    std::cout << "ENTERED PRODUCER FUNC" << std::endl;
    int count = 0;
    while(true) {
        // create a job
        struct job* new_job = new job;
        new_job->vertices = count;
        new_job->edges = "test";
        
        // add to queue
        std::cout << "Adding job: " << count << std::endl;
        job_queue.push_back(new_job);

        count ++;
        usleep(600000);
        // output from result queue
        if (!result_queue.empty()) {
            std::string output = result_queue.front();
            result_queue.pop_front();
            std::cout << "Work Complete - Result: " << output << std::endl;
        }

    }

    return NULL;
}

void* test_consumer(void* args) {
    std::cout << "ENTERED CONSUMER FUNC" << std::endl;
    while(true) {
        if (!job_queue.empty()) {
            // get job from queue
            struct job* tmp_job = new job;
            tmp_job = job_queue.front(); 
            job_queue.pop_front();

            // do work on job
            std::cout << "Working on job :" << tmp_job->vertices << std::endl;
            

            // put result into queue
            std::string result = "COMPLETE " + std::to_string(tmp_job->vertices);
            std::cout << "Pushing result for: " << tmp_job->vertices << std::endl;
            result_queue.push_back(result);
            
            // cleanup
            delete tmp_job;
        }
        
        sleep(1);
    }

    return NULL;
}


int main() {
    
    // pthread_t IO_thread;
    // pthread_t cnf_sat_thread;
    // pthread_t approx_vc1_thread;
    // pthread_t approx_vc2_thread;

    // pthread_create (&IO_thread, NULL, &IO_handler, NULL);
    // pthread_create (&cnf_sat_thread, NULL, &IO_handler, NULL);
    // pthread_create (&approx_vc1_thread, NULL, &IO_handler, NULL);
    // pthread_create (&approx_vc2_thread, NULL, &IO_handler, NULL);

    // std::cout << "waiting..." << std::endl;
    // pthread_join (IO_thread, NULL);
    // pthread_join (cnf_sat_thread, NULL);
    // pthread_join (approx_vc1_thread, NULL);
    // pthread_join (approx_vc2_thread, NULL);


    pthread_t thd_1;
    pthread_t thd_2;

    pthread_create (&thd_1, NULL, &test_producer, NULL);
    pthread_create (&thd_2, NULL, &test_consumer, NULL);

    std::cout << "waiting..." << std::endl;
    pthread_join (thd_1, NULL);
    pthread_join (thd_2, NULL);

    return 0;
}