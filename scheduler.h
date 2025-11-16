#ifndef AGNI_SCHEDULER_H
#define AGNI_SCHEDULER_H

#include <stdint.h>
#include <string>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "config.h"

// ============================================================================
// JOB STATUS ENUM
// ============================================================================
enum JobStatus {
    STATUS_QUEUED   = 0,
    STATUS_RUNNING  = 1,
    STATUS_COMPLETE = 2,
    STATUS_ERROR    = 3
};

// ============================================================================
// JOB STRUCTURE
// ============================================================================
struct Job {
    uint32_t job_id;
    std::string weapon_id;
    std::string prompt;
    int priority;
    JobStatus status;
    std::string result;
    char error_message[256];

    // Constructor
    Job() : job_id(0), priority(0), status(STATUS_QUEUED) {
        memset(error_message, 0, sizeof(error_message));
    }
};

// ============================================================================
// SCHEDULER CLASS
// ============================================================================
class Scheduler {
public:
    // Constructor/Destructor
    Scheduler();
    ~Scheduler();

    // Submit a job
    uint32_t submit_job(const std::string& weapon_id,
                        const std::string& prompt,
                        int priority = 0);

    // Get job details
    Job* get_job(uint32_t job_id);

    // Poll job status
    JobStatus poll_job(uint32_t job_id);

    // Start/Stop scheduler
    void start();
    void stop();

    // Get queue size
    size_t get_queue_size() const;

private:
    std::queue<Job> job_queue;
    std::vector<Job> job_history;
    std::vector<std::thread> workers;

    std::mutex queue_mutex;
    std::mutex history_mutex;
    std::condition_variable job_available;

    volatile bool running;
    uint32_t next_job_id;

    // Worker thread main loop
    void worker_thread();

    // Process individual job
    void process_job(Job& job);
};

#endif // AGNI_SCHEDULER_H