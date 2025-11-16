#ifndef AGNI_CONFIG_H
#define AGNI_CONFIG_H

// ============================================================================
// MODEL PARAMETERS
// ============================================================================
#define MAMBA_VOCAB_SIZE       50257
#define MAMBA_HIDDEN_SIZE      768
#define MAMBA_NUM_LAYERS       12
#define MAMBA_STATE_SIZE       16
#define MAMBA_HEAD_DIM         64

// ============================================================================
// PERFORMANCE TARGETS
// ============================================================================
#define TARGET_LATENCY_MS      54              // Target: 54ms per token
#define TARGET_MEMORY_MB       256             // Target: 256MB total
#define TARGET_POWER_W         7               // Target: 7W

// ============================================================================
// SCHEDULER/QUEUE PARAMETERS
// ============================================================================
#define MAX_QUEUE_SIZE         1000
#define MAX_WORKERS            4
#define WORKER_TIMEOUT_MS      30000           // 30 second timeout

// ============================================================================
// API SERVER PARAMETERS
// ============================================================================
#define API_PORT               8080
#define API_MAX_CONNECTIONS    100
#define API_REQUEST_TIMEOUT_MS 60000           // 60 second timeout
#define API_MAX_UPLOAD_SIZE    (50 * 1024 * 1024) // 50MB

// ============================================================================
// BUZZING BEES PARAMETERS
// ============================================================================
#define BEES_FOREMAN_RAM_ADDR  0x00000000
#define BEES_FOREMAN_RAM_SIZE  (16 * 1024 * 1024) // 16MB for AI agents
#define BEES_NUM_AGENTS        4
#define BEES_AGENT_SIZE_MB     60.5             // ~242MB total / 4

#endif // AGNI_CONFIG_H