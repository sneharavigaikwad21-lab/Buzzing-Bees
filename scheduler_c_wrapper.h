#ifndef SCHEDULER_C_WRAPPER_H
#define SCHEDULER_C_WRAPPER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// JOB STATE ENUM
// ============================================================================
typedef enum {
    JOB_STATE_QUEUED   = 0,
    JOB_STATE_RUNNING  = 1,
    JOB_STATE_COMPLETE = 2,
    JOB_STATE_ERROR    = 3
} JobState;

// ============================================================================
// JOB STRUCTURE (C-compatible)
// ============================================================================
typedef struct {
    uint32_t job_id;
    char* prompt;
    int priority;
    JobState state;
} Job_t;

// ============================================================================
// SCHEDULER C INTERFACE
// ============================================================================

// Initialize scheduler
void* scheduler_init(void);

// Submit a job to the scheduler
int scheduler_submit(void* sched, Job_t* job);

// Poll job status
int scheduler_poll(void* sched, uint32_t job_id, JobState* state);

// Cleanup scheduler
void scheduler_cleanup(void* sched);

// Get queue size
size_t scheduler_get_queue_size(void* sched);

#ifdef __cplusplus
}
#endif

#endif // SCHEDULER_C_WRAPPER_H