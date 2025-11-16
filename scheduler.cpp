#include "scheduler.h"
#include <chrono>

// ============================================================================
// CONSTRUCTOR/DESTRUCTOR
// ============================================================================
Scheduler::Scheduler() : running(false), next_job_id(1) {}

Scheduler::~Scheduler() {
    stop();
}

// ============================================================================
// SUBMIT JOB (Thread-Safe)
// ============================================================================
uint32_t Scheduler::submit_job(const std::string& weapon_id,
                            const std::string& prompt,
                            int priority) {
    // BUG FIX #6: Add lock guard to protect next_job_id and job_queue
    std::lock_guard<std::mutex> lock(queue_mutex);

    Job new_job;
    new_job.job_id = next_job_id++;
    new_job.weapon_id = weapon_id;
    new_job.prompt = prompt;
    new_job.priority = priority;
    new_job.status = STATUS_QUEUED;

    job_queue.push(new_job);

    // Also add to history for polling
    {
        std::lock_guard<std::mutex> history_lock(history_mutex);
        job_history.push_back(new_job);
    }

    job_available.notify_one();
    return new_job.job_id;
}

// ============================================================================
// GET JOB DETAILS
// ============================================================================
Job* Scheduler::get_job(uint32_t job_id) {
    std::lock_guard<std::mutex> lock(history_mutex);
    for (auto& job : job_history) {
        if (job.job_id == job_id) {
            return &job;
        }
    }
    return nullptr;
}

// ============================================================================
// POLL JOB STATUS
// ============================================================================
JobStatus Scheduler::poll_job(uint32_t job_id) {
    Job* job = get_job(job_id);
    if (job) {
        // No need to lock here, get_job already did.
        return job->status;
    }
    return STATUS_ERROR; // Represents "not found"
}

// ============================================================================
// START/STOP SCHEDULER
// ============================================================================
void Scheduler::start() {
    if (running) return;

    running = true;
    for (int i = 0; i < MAX_WORKERS; ++i) {
        workers.emplace_back(&Scheduler::worker_thread, this);
    }
}

void Scheduler::stop() {
    if (!running) return;

    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        running = false;
    }
    job_available.notify_all();

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers.clear();
}

// ============================================================================
// GET QUEUE SIZE
// ============================================================================
size_t Scheduler::get_queue_size() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(queue_mutex));
    return job_queue.size();
}

// ============================================================================
// WORKER THREAD
// ============================================================================
void Scheduler::worker_thread() {
    while (running) {
        Job current_job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            job_available.wait(lock, [this] { return !job_queue.empty() || !running; });

            if (!running && job_queue.empty()) {
                return;
            }

            current_job = job_queue.front();
            job_queue.pop();
        }

        process_job(current_job);
    }
}

// ============================================================================
// PROCESS JOB
// ============================================================================
void Scheduler::process_job(Job& job) {
    // Update status in history to RUNNING
    Job* history_job = get_job(job.job_id);
    if (history_job) {
        history_job->status = STATUS_RUNNING;
    }

    // Simulate work being done
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    job.result = "Job " + std::to_string(job.job_id) + " completed.";

    // Update final status in history to COMPLETE
    if (history_job) {
        history_job->status = STATUS_COMPLETE;
        history_job->result = job.result;
    }
}