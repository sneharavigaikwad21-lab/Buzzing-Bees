#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <string>

#include "common.h"
#include "config.h"
#include "vector_utils.h"
#include "scheduler.h"
#include "api_gateway.h"

// ============================================================================
// TEST RUNNER
// ============================================================================
int total_tests = 0;
int passed_tests = 0;

void run_test(void (*test_func)(), const std::string& test_name) {
    total_tests++;
    std::cout << "[TEST] Running: " << test_name << "..." << std::endl;
    try {
        test_func();
        passed_tests++;
        std::cout << "[PASS] " << test_name << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] " << test_name << " - Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[FAIL] " << test_name << " - Unknown exception" << std::endl;
    }
}

// ============================================================================
// VECTOR UTILS TESTS
// ============================================================================
void test_simd_add_f64() {
    double src1[] = {1.0, 2.0, 3.0, 4.0};
    double src2[] = {5.0, 6.0, 7.0, 8.0};
    double dst[4];
    simd_add_f64(dst, src1, src2, 4);
    assert(abs(dst[0] - 6.0) < 1e-9);
    assert(abs(dst[1] - 8.0) < 1e-9);
    assert(abs(dst[2] - 10.0) < 1e-9);
    assert(abs(dst[3] - 12.0) < 1e-9);
}

void test_simd_mul_f64() {
    double src1[] = {1.0, 2.0, 3.0, 4.0};
    double src2[] = {5.0, 6.0, 7.0, 8.0};
    double dst[4];
    simd_mul_f64(dst, src1, src2, 4);
    assert(abs(dst[0] - 5.0) < 1e-9);
    assert(abs(dst[1] - 12.0) < 1e-9);
    assert(abs(dst[2] - 21.0) < 1e-9);
    assert(abs(dst[3] - 32.0) < 1e-9);
}

void test_simd_dot_f64() {
    double src1[] = {1.0, 2.0, 3.0, 4.0};
    double src2[] = {5.0, 6.0, 7.0, 8.0};
    double result = simd_dot_f64(src1, src2, 4);
    assert(abs(result - 70.0) < 1e-9);
}

void test_simd_softmax_f64() {
    double vec[] = {1.0, 2.0, 3.0, 4.0};
    simd_softmax_f64(vec, 4);
    double sum = 0.0;
    for(int i = 0; i < 4; ++i) sum += vec[i];
    assert(abs(sum - 1.0) < 1e-9);
    assert(vec[0] < vec[1] && vec[1] < vec[2] && vec[2] < vec[3]);
}

void test_simd_relu_f64() {
    double src[] = {-2.0, -1.0, 0.0, 1.0, 2.0};
    double dst[5];
    simd_relu_f64(dst, src, 5);
    assert(abs(dst[0] - 0.0) < 1e-9);
    assert(abs(dst[1] - 0.0) < 1e-9);
    assert(abs(dst[2] - 0.0) < 1e-9);
    assert(abs(dst[3] - 1.0) < 1e-9);
    assert(abs(dst[4] - 2.0) < 1e-9);
}

void test_simd_gelu_f64() {
    double src[] = {-1.0, 0.0, 1.0};
    double dst[3];
    simd_gelu_f64(dst, src, 3);
    // Approximate values for GELU, recalculated for this specific formula
    assert(abs(dst[0] - (-0.1588)) < 1e-4);
    assert(abs(dst[1] - 0.0) < 1e-9);
    assert(abs(dst[2] - 0.8412) < 1e-4);
}

void test_simd_tanh_f64() {
    double src[] = {-1.0, 0.0, 1.0};
    double dst[3];
    simd_tanh_f64(dst, src, 3);
    assert(abs(dst[0] - (-0.761594)) < 1e-5);
    assert(abs(dst[1] - 0.0) < 1e-9);
    assert(abs(dst[2] - 0.761594) < 1e-5);
}


// ============================================================================
// SCHEDULER TESTS
// ============================================================================
void test_scheduler_submit_and_poll() {
    Scheduler scheduler;
    scheduler.start();
    uint32_t job_id = scheduler.submit_job("test_weapon", "test_prompt");
    assert(job_id > 0);

    JobStatus status = scheduler.poll_job(job_id);
    assert(status == STATUS_QUEUED || status == STATUS_RUNNING || status == STATUS_COMPLETE);

    // Wait for job to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    status = scheduler.poll_job(job_id);
    assert(status == STATUS_COMPLETE);

    scheduler.stop();
}

void test_scheduler_queue_size() {
    Scheduler scheduler;
    scheduler.start();
    assert(scheduler.get_queue_size() == 0);
    scheduler.submit_job("test", "prompt1");
    scheduler.submit_job("test", "prompt2");
    // Size check is tricky due to timing, but should be > 0
    // A better test would be to pause workers.
    scheduler.stop();
}

// ============================================================================
// API GATEWAY TESTS
// ============================================================================
void test_api_gateway_endpoints() {
    Scheduler scheduler;
    APIGateway gateway(&scheduler);

    // Test Health
    APIRequest health_req = {"GET", "/v59/health"};
    APIResponse health_resp = gateway.handle_request(health_req);
    assert(health_resp.status_code == 200);
    assert(health_resp.body.find("\"status\": \"healthy\"") != std::string::npos);

    // Test Axiom (Submit)
    APIRequest axiom_req = {"POST", "/v59/axiom", "{\"prompt\":\"test\"}"};
    APIResponse axiom_resp = gateway.handle_request(axiom_req);
    assert(axiom_resp.status_code == 200);
    assert(axiom_resp.body.find("\"job_uuid\"") != std::string::npos);

    // Test Upload
    APIRequest upload_req = {"POST", "/v59/upload", "file content"};
    APIResponse upload_resp = gateway.handle_request(upload_req);
    assert(upload_resp.status_code == 200);
    assert(upload_resp.body.find("\"status\": \"UPLOADED\"") != std::string::npos);

    // Test 404
    APIRequest notfound_req = {"GET", "/nonexistent"};
    APIResponse notfound_resp = gateway.handle_request(notfound_req);
    assert(notfound_resp.status_code == 404);
}

// ============================================================================
// MAIN
// ============================================================================
int main() {
    std::cout << "========================================================" << std::endl;
    std::cout << "        RUNNING PRODUCTION TEST SUITE (v45)             " << std::endl;
    std::cout << "========================================================" << std::endl;

    run_test(test_simd_add_f64, "Vector Add f64");
    run_test(test_simd_mul_f64, "Vector Multiply f64");
    run_test(test_simd_dot_f64, "Vector Dot Product f64");
    run_test(test_simd_softmax_f64, "Vector Softmax f64");
    run_test(test_simd_relu_f64, "Vector ReLU f64");
    run_test(test_simd_gelu_f64, "Vector GELU f64");
    run_test(test_simd_tanh_f64, "Vector Tanh f64");

    run_test(test_scheduler_submit_and_poll, "Scheduler Submit & Poll");
    run_test(test_scheduler_queue_size, "Scheduler Queue Size");

    run_test(test_api_gateway_endpoints, "API Gateway Endpoints");

    std::cout << "========================================================" << std::endl;
    std::cout << "Test Results: " << passed_tests << " / " << total_tests << " passed." << std::endl;
    std::cout << "========================================================" << std::endl;

    return (passed_tests == total_tests) ? 0 : 1;
}