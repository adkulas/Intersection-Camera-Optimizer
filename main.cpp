
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


struct job {
  int vertices;
  std::vector< std::pair<int,int> > edges;
};

// kind of ugly... maybe consider moving into a queue class
pthread_mutex_t job_queue1_mutex = PTHREAD_MUTEX_INITIALIZER;  //mutex between cnf_sat and IO_handler
pthread_mutex_t job_queue2_mutex = PTHREAD_MUTEX_INITIALIZER;  //mutex between approx_vc_1 and IO_handler
pthread_mutex_t job_queue3_mutex = PTHREAD_MUTEX_INITIALIZER;  //mutex between approx_vc_2 IO_handler
pthread_mutex_t result_queue1_mutex = PTHREAD_MUTEX_INITIALIZER;  //mutex between cnf_sat and IO_handler
pthread_mutex_t result_queue2_mutex = PTHREAD_MUTEX_INITIALIZER;  //mutex between approx_vc_1 and IO_handler
pthread_mutex_t result_queue3_mutex = PTHREAD_MUTEX_INITIALIZER;  //mutex between approx_vc_2 IO_handler

std::list<job*> job_queue1;  // job queue for cnf_sat_vc
std::list<job*> job_queue2;  // job queue for approx_vc_1
std::list<job*> job_queue3;  // job queue for approx_vc_2

std::list<std::string> result_queue1; // result queue for cnf_sat_vc
std::list<std::string> result_queue2; // result queue for approx_vc_1
std::list<std::string> result_queue3; // result queue for approx_vc_2


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
            {
                std::cin >> edges_input;
                // std::cout << "E " << edges_input << std::endl;
                parsed_edges = parse(edges_input);
                cnf_sat_vc(vertices, parsed_edges);
                approx_vc_1(vertices, parsed_edges);
                approx_vc_2(vertices, parsed_edges);

                struct job* incoming_job;

                // add to queue 1
                incoming_job = new job;
                incoming_job->vertices = vertices;
                incoming_job->edges = parsed_edges;
                pthread_mutex_lock (&job_queue1_mutex);
                job_queue1.push_back(incoming_job);
                pthread_mutex_unlock (&job_queue1_mutex);

                // // add to queue 2
                // incoming_job = new job;
                // incoming_job->vertices = vertices;
                // incoming_job->edges = parsed_edges;
                // pthread_mutex_lock (&job_queue2_mutex);
                // job_queue2.push_back(incoming_job);
                // pthread_mutex_unlock (&job_queue2_mutex);

                // // add to queue 3
                // incoming_job = new job;
                // incoming_job->vertices = vertices;
                // incoming_job->edges = parsed_edges;
                // pthread_mutex_lock (&job_queue3_mutex);
                // job_queue3.push_back(incoming_job);
                // pthread_mutex_unlock (&job_queue3_mutex);

                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;
            }

            default:
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Error: command not recognized" << std::endl;
        }

        pthread_mutex_lock (&result_queue1_mutex);
        pthread_mutex_lock (&result_queue2_mutex);
        pthread_mutex_lock (&result_queue3_mutex);

        while(!result_queue1.empty() && !result_queue1.empty() && !result_queue1.empty()) {
            std::string result = result_queue1.front();
            result_queue1.pop_front();
            std::cout << "CNF-SAT-VC: " << result << std::endl;
        }

        pthread_mutex_unlock (&result_queue1_mutex);
        pthread_mutex_unlock (&result_queue2_mutex);
        pthread_mutex_unlock (&result_queue3_mutex); 

    }
 

    return NULL;
}


void* calc_cnf_sat_vc(void* args) {
    
    std::clog << "thread calc cnf started..." << std::endl;
    
    while (true) {
        struct job* retrieved_job;
        std::string result;

        // get job from queue, use mutex for thread safety
        pthread_mutex_lock (&job_queue1_mutex);
        if (!job_queue1.empty()) {
            retrieved_job = job_queue1.front(); 
            job_queue1.pop_front();

            // compute result
            cnf_sat_vc(retrieved_job->vertices, retrieved_job->edges);
            result = "Finished Job: " + std::to_string(retrieved_job->vertices); 

            //write result to result queue, use mutex for thread safety
            pthread_mutex_lock (&result_queue1_mutex);
            result_queue1.push_back(result);
            pthread_mutex_unlock (&result_queue1_mutex);
            
            // cleanup memory taken by job
            
        }
        pthread_mutex_unlock (&job_queue1_mutex);
        delete retrieved_job;
        retrieved_job = NULL;
    }

    return NULL;
}


void* calc_aprox_vc_1(void* args) {
    return NULL;
}


void* calc_approx_vc_2(void* args) {
    return NULL;
}


int main() {
    
    pthread_t IO_thread;
    pthread_t cnf_sat_thread;
    pthread_t approx_vc1_thread;
    pthread_t approx_vc2_thread;

    pthread_create (&IO_thread, NULL, &IO_handler, NULL);
    pthread_create (&cnf_sat_thread, NULL, &calc_cnf_sat_vc, NULL);
    pthread_create (&approx_vc1_thread, NULL, &calc_aprox_vc_1, NULL);
    pthread_create (&approx_vc2_thread, NULL, &calc_approx_vc_2, NULL);

    std::cout << "all threads started..." << std::endl;
    pthread_join (IO_thread, NULL);
    pthread_join (cnf_sat_thread, NULL);
    pthread_join (approx_vc1_thread, NULL);
    pthread_join (approx_vc2_thread, NULL);


    // pthread_t thd_1;
    // pthread_t thd_2;

    // pthread_create (&thd_1, NULL, &test_producer, NULL);
    // pthread_create (&thd_2, NULL, &test_consumer, NULL);

    // std::cout << "waiting..." << std::endl;
    // pthread_join (thd_1, NULL);
    // pthread_join (thd_2, NULL);

    return 0;
}