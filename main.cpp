
#include <minisat/core/SolverTypes.h>
#include <minisat/core/Solver.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <regex>
#include <vector>
#include <list>
#include <iostream>

#include "cnf-sat-vc.hpp"
#include "approx_vc.hpp"

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

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


std::string cnf_sat_vc(int vertices, std::vector< std::pair<int,int> > edges) {
    std::string result;
    VertexCover v_cover = VertexCover(vertices, edges);
    result = v_cover.lin_search_vcover();

    return result;
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
                // std::cout << "CNF-SAT-VC: " << cnf_sat_vc(vertices, parsed_edges) << std::endl;
                // std::cout << "APPROX-VC-1: " << approx_vc_1(vertices, parsed_edges) << std::endl;
                // std::cout << "APPROX-VC-2: " << approx_vc_2(vertices, parsed_edges) << std::endl;

                struct job* incoming_job;

                // add to queue 1
                incoming_job = new job;
                incoming_job->vertices = vertices;
                incoming_job->edges = parsed_edges;
                pthread_mutex_lock (&job_queue1_mutex);
                job_queue1.push_back(incoming_job);
                pthread_mutex_unlock (&job_queue1_mutex);

                // add to queue 2
                incoming_job = new job;
                incoming_job->vertices = vertices;
                incoming_job->edges = parsed_edges;
                pthread_mutex_lock (&job_queue2_mutex);
                job_queue2.push_back(incoming_job);
                pthread_mutex_unlock (&job_queue2_mutex);

                // add to queue 3
                incoming_job = new job;
                incoming_job->vertices = vertices;
                incoming_job->edges = parsed_edges;
                pthread_mutex_lock (&job_queue3_mutex);
                job_queue3.push_back(incoming_job);
                pthread_mutex_unlock (&job_queue3_mutex);

                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                break;
            }

            default:
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Error: command not recognized" << std::endl;
        }
    }

    return NULL;
}


void* output_handler(void* args) {
        
    while(true) {  
        pthread_mutex_lock (&result_queue1_mutex);
        pthread_mutex_lock (&result_queue2_mutex);
        pthread_mutex_lock (&result_queue3_mutex);

        // write results to console in required order
        while(!result_queue1.empty() && !result_queue2.empty() && !result_queue3.empty()) {
            std::string result;
            
            result = result_queue1.front();
            result_queue1.pop_front();
            std::cout << result << std::endl;

            result = result_queue2.front();
            result_queue2.pop_front();
            std::cout << result << std::endl;

            result = result_queue3.front();
            result_queue3.pop_front();
            std::cout << result << std::endl;
        }
        pthread_mutex_unlock (&result_queue1_mutex);
        pthread_mutex_unlock (&result_queue2_mutex);
        pthread_mutex_unlock (&result_queue3_mutex); 
    }

    return NULL;
}


void* calc_cnf_sat_vc(void* args) {
    
    clockid_t clock_id;
    int job_number = 1;

    while (true) {
        struct job* retrieved_job = NULL;
        std::string result;

        // get job from queue, use mutex for thread safety
        pthread_mutex_lock (&job_queue1_mutex);
        if (!job_queue1.empty()) {
            retrieved_job = job_queue1.front(); 
            job_queue1.pop_front();

            // time start
            pthread_getcpuclockid(pthread_self(), &clock_id);
            struct timespec ts_start;
            clock_gettime(clock_id, &ts_start);

            // compute result
            result = "CNF-SAT-VC: " + cnf_sat_vc(retrieved_job->vertices, retrieved_job->edges);
            
            //time end
            struct timespec ts_end;
            clock_gettime(clock_id, &ts_end);
            long double elapsed_time_us = ((long double)ts_end.tv_sec*1000000 + (long double)ts_end.tv_nsec/1000.0) - ((long double)ts_start.tv_sec*1000000 + (long double)ts_start.tv_nsec/1000.0);
            std::clog << "CNF-SAT-VC,Job_num," << job_number << "," 
                      << "time(us)," << elapsed_time_us << std::endl;

            //write result to result queue, use mutex for thread safety
            pthread_mutex_lock (&result_queue1_mutex);
            result_queue1.push_back(result);
            pthread_mutex_unlock (&result_queue1_mutex);
            
            job_number++;
            
        }
        pthread_mutex_unlock (&job_queue1_mutex);

        // cleanup memory taken by job
        delete retrieved_job;
    }

    return NULL;
}


void* calc_aprox_vc_1(void* args) {
        
    clockid_t clock_id;
    int job_number = 1;
    
    while (true) {
        struct job* retrieved_job = NULL;
        std::string result;

        // get job from queue, use mutex for thread safety
        pthread_mutex_lock (&job_queue2_mutex);
        if (!job_queue2.empty()) {
            retrieved_job = job_queue2.front(); 
            job_queue2.pop_front();

            // time start
            pthread_getcpuclockid(pthread_self(), &clock_id);
            struct timespec ts_start;
            clock_gettime(clock_id, &ts_start);

            // compute result
            result = "APPROX-VC-1: " + approx_vc_1(retrieved_job->vertices, retrieved_job->edges);

            //time end
            struct timespec ts_end;
            clock_gettime(clock_id, &ts_end);
            long double elapsed_time_us = ((long double)ts_end.tv_sec*1000000 + (long double)ts_end.tv_nsec/1000.0) - ((long double)ts_start.tv_sec*1000000 + (long double)ts_start.tv_nsec/1000.0);
            std::clog << "APPROX-VC-1,Job_num," << job_number << "," 
                      << "time(us)," << elapsed_time_us << std::endl;

            //write result to result queue, use mutex for thread safety
            pthread_mutex_lock (&result_queue2_mutex);
            result_queue2.push_back(result);
            pthread_mutex_unlock (&result_queue2_mutex);  

            job_number++;
        }
        pthread_mutex_unlock (&job_queue2_mutex);
        
        // cleanup memory taken by job
        delete retrieved_job;
    }

    return NULL;
}


void* calc_approx_vc_2(void* args) {
        
    clockid_t clock_id;
    int job_number = 1;
    
    while (true) {
        struct job* retrieved_job = NULL;
        std::string result;

        // get job from queue, use mutex for thread safety
        pthread_mutex_lock (&job_queue3_mutex);
        if (!job_queue3.empty()) {
            retrieved_job = job_queue3.front(); 
            job_queue3.pop_front();

            // time start
            pthread_getcpuclockid(pthread_self(), &clock_id);
            struct timespec ts_start;
            clock_gettime(clock_id, &ts_start);

            // compute result
            result = "APPROX-VC-2: " + approx_vc_2(retrieved_job->vertices, retrieved_job->edges);

            //time end
            struct timespec ts_end;
            clock_gettime(clock_id, &ts_end);
            long double elapsed_time_us = ((long double)ts_end.tv_sec*1000000 + (long double)ts_end.tv_nsec/1000.0) - ((long double)ts_start.tv_sec*1000000 + (long double)ts_start.tv_nsec/1000.0);
            std::clog << "APPROX-VC-2,Job_num," << job_number << "," 
                      << "time(us)," << elapsed_time_us << std::endl;


            //write result to result queue, use mutex for thread safety
            pthread_mutex_lock (&result_queue3_mutex);
            result_queue3.push_back(result);
            pthread_mutex_unlock (&result_queue3_mutex);
            

            job_number++;
        }
        pthread_mutex_unlock (&job_queue3_mutex);
        // cleanup memory taken by job
        delete retrieved_job;
    }
    
    return NULL;
}


int main() {
    
    pthread_t IO_thread;
    pthread_t out_thread;
    pthread_t cnf_sat_thread;
    pthread_t approx_vc1_thread;
    pthread_t approx_vc2_thread;

    pthread_create (&IO_thread, NULL, &IO_handler, NULL);
    pthread_create (&out_thread, NULL, &output_handler, NULL);
    pthread_create (&cnf_sat_thread, NULL, &calc_cnf_sat_vc, NULL);
    pthread_create (&approx_vc1_thread, NULL, &calc_aprox_vc_1, NULL);
    pthread_create (&approx_vc2_thread, NULL, &calc_approx_vc_2, NULL);

    pthread_join (IO_thread, NULL);
    pthread_join (out_thread, NULL);
    pthread_join (cnf_sat_thread, NULL);
    pthread_join (approx_vc1_thread, NULL);
    pthread_join (approx_vc2_thread, NULL);

    return 0;
}