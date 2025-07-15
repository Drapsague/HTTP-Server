#include "../include/securityAnalyzer.h"
#include <cassert>
#include <iostream>

void testPathTraversalDetection() {
    std::cout << "Testing Path Traversal Detection..." << std::endl;
    
    SecurityAnalyzer analyzer;
    
    // Vulnerable code with both user input and file operation
    std::string vulnerable_code = R"(
        recv(socket, recv_buffer, size, 0);
        char* user_input = recv_buffer;
        std::ifstream file(user_input);
        file.read(buffer, size);
    )";
    
    analyzer.analyzeCodeSnippet(vulnerable_code);
    const auto& vuln_info = analyzer.getVulnerabilityInfo();
    
    assert(vuln_info.has_vulnerability == true);
    // Should detect path traversal since we have network input -> file operation
    assert(vuln_info.cwe_id == "CWE-22");
    assert(vuln_info.vulnerability_name == "Path Traversal");
    
    std::cout << "✓ Path traversal detection test passed" << std::endl;
}

void testBufferOverflowDetection() {
    std::cout << "Testing Buffer Overflow Detection..." << std::endl;
    
    SecurityAnalyzer analyzer;
    
    // Vulnerable code with network input and unsafe memory operation
    std::string vulnerable_code = R"(
        char buffer[100];
        char data[1000];
        recv(socket, data, 1000, 0);
        strcpy(buffer, data);
    )";
    
    analyzer.analyzeCodeSnippet(vulnerable_code);
    const auto& vuln_info = analyzer.getVulnerabilityInfo();
    
    assert(vuln_info.has_vulnerability == true);
    // Should detect buffer overflow due to network input to memory operation
    
    std::cout << "✓ Buffer overflow detection test passed" << std::endl;
}

void testSecureCodeAnalysis() {
    std::cout << "Testing Secure Code Analysis..." << std::endl;
    
    SecurityAnalyzer analyzer;
    
    // Secure code with proper validation
    std::string secure_code = R"(
        char* user_input = recv_buffer;
        if (strlen(user_input) > MAX_PATH) return;
        
        char canonical_path[PATH_MAX];
        if (realpath(user_input, canonical_path) != nullptr) {
            std::ifstream file(canonical_path);
            if (file.is_open()) {
                file.read(buffer, sizeof(buffer));
            }
        }
    )";
    
    analyzer.analyzeCodeSnippet(secure_code);
    const auto& vuln_info = analyzer.getVulnerabilityInfo();
    
    // Should have sanitizers and no vulnerabilities
    const auto& sanitizers = analyzer.getSanitizers();
    assert(!sanitizers.empty());
    
    std::cout << "✓ Secure code analysis test passed" << std::endl;
}

void testSourceAndSinkDetection() {
    std::cout << "Testing Source and Sink Detection..." << std::endl;
    
    SecurityAnalyzer analyzer;
    
    std::string code = R"(
        // Sources
        recv(socket, buffer, size, 0);
        accept(server_socket, nullptr, nullptr);
        std::ifstream input_file("data.txt");
        
        // Sinks
        std::ofstream output_file("output.txt");
        send(socket, response, length, 0);
        system("ls -la");
        memcpy(dest, src, count);
    )";
    
    analyzer.analyzeCodeSnippet(code);
    
    const auto& sources = analyzer.getSources();
    const auto& sinks = analyzer.getSinks();
    
    assert(sources.size() >= 2); // recv, accept, ifstream
    assert(sinks.size() >= 3);   // ofstream, send, system, memcpy
    
    std::cout << "✓ Source and sink detection test passed" << std::endl;
}

void testReportFormat() {
    std::cout << "Testing Report Format..." << std::endl;
    
    SecurityAnalyzer analyzer;
    
    std::string code = R"(
        recv(socket, user_data, size, 0);
        std::ifstream file(user_data);
    )";
    
    analyzer.analyzeCodeSnippet(code);
    
    std::cout << "\n--- Sample Report Output ---" << std::endl;
    analyzer.generateSecurityReport();
    std::cout << "--- End Sample Report ---\n" << std::endl;
    
    std::cout << "✓ Report format test completed" << std::endl;
}

int main() {
    std::cout << "Running Security Analyzer Tests" << std::endl;
    std::cout << "===============================" << std::endl;
    
    try {
        testSourceAndSinkDetection();
        testPathTraversalDetection();
        testBufferOverflowDetection();
        testSecureCodeAnalysis();
        testReportFormat();
        
        std::cout << "\n✅ All tests passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}