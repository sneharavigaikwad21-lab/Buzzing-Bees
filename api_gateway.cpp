#include "api_gateway.h"
#include "common.h"
#include <sstream>
#include <cstring>

// ============================================================================
// CONSTRUCTOR
// ============================================================================
APIGateway::APIGateway(Scheduler* sched)
    : scheduler(sched), server_running(false) {
    LOG_INFO("API Gateway initialized");
}

// ============================================================================
// DESTRUCTOR
// ============================================================================
APIGateway::~APIGateway() {
    if (server_running) {
        stop_server();
    }
    LOG_INFO("API Gateway destroyed");
}

// ============================================================================
// REQUEST ROUTER
// ============================================================================
APIResponse APIGateway::handle_request(const APIRequest& req) {
    APIResponse resp;
    resp.headers["Content-Type"] = "application/json";

    // BUG FIX: Add null pointer check
    if (!scheduler) {
        resp.status_code = 500;
        resp.body = "{\"error\": \"Scheduler not initialized\"}";
        return resp;
    }

    // Route to appropriate handler
    if (req.method == "POST" && req.endpoint == "/v59/axiom") {
        return handle_axiom(req);
    } else if (req.method == "POST" && req.endpoint == "/v59/upload") {
        return handle_upload(req);
    } else if (req.method == "GET" && req.endpoint.find("/v59/status/") == 0) {
        return handle_status(req);
    } else if (req.method == "GET" && req.endpoint == "/v59/health") {
        return handle_health(req);
    }

    // 404 Not Found
    resp.status_code = 404;
    resp.body = "{\"error\": \"Endpoint not found\"}";
    return resp;
}

// ============================================================================
// AXIOM INFERENCE ENDPOINT
// ============================================================================
APIResponse APIGateway::handle_axiom(const APIRequest& req) {
    APIResponse resp;
    resp.status_code = 200;

    // BUG FIX: Proper error handling for null scheduler
    if (!scheduler) {
        resp.status_code = 500;
        resp.body = "{\"error\": \"Scheduler unavailable\"}";
        return resp;
    }

    // Parse prompt from request body (simplified JSON parsing)
    std::string job_id_str = "WEAPON_AXIOM_001";
    std::string prompt = req.body;

    // Submit job to scheduler
    uint32_t job_id = scheduler->submit_job(job_id_str, prompt, 0);

    // Build JSON response
    std::stringstream ss;
    ss << "{\"job_uuid\": " << job_id << ", \"status\": \"QUEUED\", "
       << "\"endpoint\": \"/v59/status/" << job_id << "\"}";

    resp.body = ss.str();
    LOG_INFO("Axiom job submitted: %u", job_id);

    return resp;
}

// ============================================================================
// FILE UPLOAD ENDPOINT
// ============================================================================
APIResponse APIGateway::handle_upload(const APIRequest& req) {
    APIResponse resp;
    resp.status_code = 200;

    // BUG FIX: Validate request size
    if (req.body.size() > API_MAX_UPLOAD_SIZE) {
        resp.status_code = 413;  // Payload Too Large
        resp.body = "{\"error\": \"File size exceeds limit\"}";
        return resp;
    }

    // Generate upload ID
    static uint32_t upload_counter = 0;
    upload_counter++;

    std::stringstream ss;
    ss << "{\"file_upload_id\": \"file_" << upload_counter << "\", "
       << "\"size_mb\": " << (req.body.size() / (1024.0 * 1024.0)) << ", "
       << "\"status\": \"UPLOADED\"}";

    resp.body = ss.str();
    LOG_INFO("File uploaded: %zu bytes", req.body.size());

    return resp;
}

// ============================================================================
// STATUS CHECK ENDPOINT
// ============================================================================
APIResponse APIGateway::handle_status(const APIRequest& req) {
    APIResponse resp;
    resp.status_code = 200;

    // BUG FIX: Proper null checks
    if (!scheduler) {
        resp.status_code = 500;
        resp.body = "{\"error\": \"Scheduler unavailable\"}";
        return resp;
    }

    // Extract job_id from endpoint: "/v59/status/<job_id>"
    std::string endpoint = req.endpoint;
    std::string prefix = "/v59/status/";

    if (endpoint.find(prefix) != 0) {
        resp.status_code = 400;
        resp.body = "{\"error\": \"Invalid status endpoint format\"}";
        return resp;
    }

    uint32_t job_id = 0;
    try {
        std::string job_id_str = endpoint.substr(prefix.length());
        job_id = std::stoul(job_id_str);
    } catch (...) {
        resp.status_code = 400;
        resp.body = "{\"error\": \"Invalid job_id\"}";
        return resp;
    }

    // Poll scheduler
    JobStatus status = scheduler->poll_job(job_id);

    std::stringstream ss;
    ss << "{\"job_id\": " << job_id << ", \"status\": " << (int)status << "}";
    resp.body = ss.str();

    return resp;
}

// ============================================================================
// HEALTH CHECK ENDPOINT
// ============================================================================
APIResponse APIGateway::handle_health(const APIRequest& req) {
    APIResponse resp;
    resp.status_code = 200;

    // BUG FIX: Add scheduler health check
    if (!scheduler) {
        resp.status_code = 503;
        resp.body = "{\"status\": \"unhealthy\", \"error\": \"Scheduler unavailable\"}";
        return resp;
    }

    std::stringstream ss;
    ss << "{\"status\": \"healthy\", "
       << "\"queue_size\": " << scheduler->get_queue_size() << ", "
       << "\"uptime_s\": 0}";

    resp.body = ss.str();
    return resp;
}

// ============================================================================
// SERVER LIFECYCLE
// ============================================================================
void APIGateway::start_server(int port) {
    server_running = true;
    LOG_INFO("API Gateway listening on port %d", port);
}

void APIGateway::stop_server() {
    server_running = false;
    LOG_INFO("API Gateway stopped");
}