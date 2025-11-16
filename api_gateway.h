#ifndef AGNI_API_GATEWAY_H
#define AGNI_API_GATEWAY_H

#include <string>
#include <map>
#include "scheduler.h"

// ============================================================================
// HTTP REQUEST STRUCTURE
// ============================================================================
struct APIRequest {
    std::string method;
    std::string endpoint;
    std::string body;
    std::map<std::string, std::string> headers;
};

// ============================================================================
// HTTP RESPONSE STRUCTURE
// ============================================================================
struct APIResponse {
    int status_code;
    std::string body;
    std::map<std::string, std::string> headers;

    // Constructor
    APIResponse() : status_code(200) {}
};

// ============================================================================
// API GATEWAY CLASS
// ============================================================================
class APIGateway {
public:
    // Constructor/Destructor
    APIGateway(Scheduler* scheduler);
    ~APIGateway();

    // Handle incoming HTTP request
    APIResponse handle_request(const APIRequest& req);

    // Server lifecycle
    void start_server(int port);
    void stop_server();

private:
    Scheduler* scheduler;
    bool server_running;

    // Endpoint handlers
    APIResponse handle_axiom(const APIRequest& req);
    APIResponse handle_upload(const APIRequest& req);
    APIResponse handle_status(const APIRequest& req);
    APIResponse handle_health(const APIRequest& req);
};

#endif // AGNI_API_GATEWAY_H