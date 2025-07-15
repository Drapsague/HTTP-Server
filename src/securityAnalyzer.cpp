#include "../include/securityAnalyzer.h"
#include <algorithm>
#include <regex>
#include <sstream>

SecurityAnalyzer::SecurityAnalyzer() {
    reset();
}

void SecurityAnalyzer::reset() {
    sources.clear();
    sinks.clear();
    sanitizers.clear();
    unsanitized_flows.clear();
    vulnerability_info = {false, "", "", ""};
}

void SecurityAnalyzer::analyzeCodeSnippet(const std::string& code_snippet) {
    reset();
    
    identifySources(code_snippet);
    identifySinks(code_snippet);
    identifySanitizers(code_snippet);
    analyzeDataFlows(code_snippet);
    performVulnerabilityAnalysis();
}

bool SecurityAnalyzer::containsPattern(const std::string& code, const std::string& pattern) {
    return code.find(pattern) != std::string::npos;
}

std::vector<size_t> SecurityAnalyzer::findPatternOccurrences(const std::string& code, const std::string& pattern) {
    std::vector<size_t> positions;
    size_t pos = code.find(pattern, 0);
    while (pos != std::string::npos) {
        positions.push_back(pos);
        pos = code.find(pattern, pos + 1);
    }
    return positions;
}

void SecurityAnalyzer::identifySources(const std::string& code_snippet) {
    int source_id = 1;
    
    // Network input sources
    if (containsPattern(code_snippet, "recv(")) {
        sources.push_back({source_id++, "Network data received via recv()", "recv() function call", "network_input"});
    }
    
    if (containsPattern(code_snippet, "accept(")) {
        sources.push_back({source_id++, "Client connection accepted", "accept() function call", "network_input"});
    }
    
    // User input from HTTP requests or buffers that contain received data
    if (containsPattern(code_snippet, "m_recvBuffer") || 
        containsPattern(code_snippet, "recv_buffer") ||
        containsPattern(code_snippet, "user_input")) {
        sources.push_back({source_id++, "User input data from buffer", "user input buffer usage", "user_input"});
    }
    
    // File input sources
    if (containsPattern(code_snippet, "std::ifstream") || containsPattern(code_snippet, "ifstream")) {
        sources.push_back({source_id++, "File content read from filesystem", "ifstream operations", "file_input"});
    }
    
    // Command line arguments
    if (containsPattern(code_snippet, "argv") || containsPattern(code_snippet, "argc")) {
        sources.push_back({source_id++, "Command line arguments", "argv/argc parameters", "user_input"});
    }
    
    // Environment variables
    if (containsPattern(code_snippet, "getenv(")) {
        sources.push_back({source_id++, "Environment variable", "getenv() function call", "environment_input"});
    }
}

void SecurityAnalyzer::identifySinks(const std::string& code_snippet) {
    int sink_id = 1;
    
    // File operations
    if (containsPattern(code_snippet, "std::ifstream") || containsPattern(code_snippet, "ifstream")) {
        sinks.push_back({sink_id++, "File system access for reading", "ifstream file operations", "file_operation"});
    }
    
    if (containsPattern(code_snippet, "std::ofstream") || containsPattern(code_snippet, "ofstream")) {
        sinks.push_back({sink_id++, "File system access for writing", "ofstream file operations", "file_operation"});
    }
    
    if (containsPattern(code_snippet, "fopen(")) {
        sinks.push_back({sink_id++, "File opened using fopen()", "fopen() function call", "file_operation"});
    }
    
    // Network output
    if (containsPattern(code_snippet, "send(")) {
        sinks.push_back({sink_id++, "Data sent over network", "send() function call", "network_output"});
    }
    
    // System calls
    if (containsPattern(code_snippet, "system(")) {
        sinks.push_back({sink_id++, "System command execution", "system() function call", "system_call"});
    }
    
    if (containsPattern(code_snippet, "exec")) {
        sinks.push_back({sink_id++, "Process execution", "exec family function call", "system_call"});
    }
    
    // Memory operations
    if (containsPattern(code_snippet, "memcpy(")) {
        sinks.push_back({sink_id++, "Memory copy operation", "memcpy() function call", "memory_operation"});
    }
    
    if (containsPattern(code_snippet, "strcpy(")) {
        sinks.push_back({sink_id++, "String copy operation", "strcpy() function call", "memory_operation"});
    }
    
    if (containsPattern(code_snippet, "sprintf(")) {
        sinks.push_back({sink_id++, "String formatting operation", "sprintf() function call", "memory_operation"});
    }
}

void SecurityAnalyzer::identifySanitizers(const std::string& code_snippet) {
    int sanitizer_id = 1;
    
    // Input validation
    if ((containsPattern(code_snippet, "strlen(") && containsPattern(code_snippet, "MAX_")) || 
        (containsPattern(code_snippet, "size") && containsPattern(code_snippet, "check"))) {
        sanitizers.push_back({sanitizer_id++, "Length validation check", "size checking logic", "length_validation"});
    }
    
    // Path sanitization
    if (containsPattern(code_snippet, "realpath(")) {
        sanitizers.push_back({sanitizer_id++, "Path canonicalization", "realpath() function call", "path_sanitization"});
    }
    
    if (containsPattern(code_snippet, "..") && containsPattern(code_snippet, "check")) {
        sanitizers.push_back({sanitizer_id++, "Path traversal check", "directory traversal validation", "path_sanitization"});
    }
    
    // Buffer boundary checks
    if (containsPattern(code_snippet, "sizeof(") || containsPattern(code_snippet, "bounds")) {
        sanitizers.push_back({sanitizer_id++, "Buffer boundary validation", "size boundary checking", "buffer_validation"});
    }
    
    // Null pointer checks
    if (containsPattern(code_snippet, "!= nullptr") || containsPattern(code_snippet, "!= NULL")) {
        sanitizers.push_back({sanitizer_id++, "Null pointer validation", "null pointer checking", "null_validation"});
    }
}

bool SecurityAnalyzer::isPathTraversalVulnerable(const std::string& code) {
    // Check for file operations with user input without proper validation
    bool has_file_ops = containsPattern(code, "ifstream") || containsPattern(code, "fopen");
    bool has_user_input = containsPattern(code, "recv") || containsPattern(code, "m_recvBuffer");
    bool lacks_path_validation = !containsPattern(code, "realpath") && 
                                !containsPattern(code, "..") && 
                                !containsPattern(code, "path") && 
                                !containsPattern(code, "validate");
    
    return has_file_ops && has_user_input && lacks_path_validation;
}

bool SecurityAnalyzer::isBufferOverflowVulnerable(const std::string& code) {
    // Check for unsafe string operations
    bool has_unsafe_funcs = containsPattern(code, "strcpy") || 
                           containsPattern(code, "sprintf") || 
                           containsPattern(code, "memcpy");
    
    bool lacks_bounds_check = !containsPattern(code, "sizeof") && 
                             !containsPattern(code, "strlen") && 
                             !containsPattern(code, "bounds");
    
    return has_unsafe_funcs && lacks_bounds_check;
}

bool SecurityAnalyzer::hasUnsafeStringOperations(const std::string& code) {
    return containsPattern(code, "strcpy") || 
           containsPattern(code, "sprintf") || 
           containsPattern(code, "gets(") ||
           containsPattern(code, "strcat");
}

void SecurityAnalyzer::analyzeDataFlows(const std::string& /* code_snippet */) {
    // Analyze flows from sources to sinks
    for (const auto& source : sources) {
        for (const auto& sink : sinks) {
            bool is_sanitized = false;
            
            // Check if there's a sanitizer between source and sink
            for (const auto& sanitizer : sanitizers) {
                // Simple heuristic: if sanitizer type matches the vulnerability type
                if ((source.type == "user_input" && sanitizer.function_name.find("validation") != std::string::npos) ||
                    (sink.type == "file_operation" && sanitizer.function_name.find("path") != std::string::npos) ||
                    (sink.type == "memory_operation" && sanitizer.function_name.find("buffer") != std::string::npos)) {
                    is_sanitized = true;
                    break;
                }
            }
            
            if (!is_sanitized) {
                std::string vulnerability_reason;
                
                if (source.type == "user_input" && sink.type == "file_operation") {
                    vulnerability_reason = "User input flows directly to file operations without path validation, enabling path traversal attacks";
                } else if (source.type == "network_input" && sink.type == "memory_operation") {
                    vulnerability_reason = "Network input flows to memory operations without proper bounds checking, enabling buffer overflow";
                } else if (source.type == "user_input" && sink.type == "system_call") {
                    vulnerability_reason = "User input flows to system calls without sanitization, enabling command injection";
                } else {
                    vulnerability_reason = "Untrusted data flows to sensitive operation without proper validation";
                }
                
                unsanitized_flows.push_back({
                    source.id, 
                    sink.id, 
                    vulnerability_reason,
                    source.description + " -> " + sink.description
                });
            }
        }
    }
}

void SecurityAnalyzer::performVulnerabilityAnalysis() {
    std::stringstream analysis_details;
    bool has_vulnerability = false;
    std::string primary_cwe = "";
    std::string primary_vuln_name = "";
    
    // Analyze the actual code for vulnerability patterns
    bool has_path_traversal = false;
    bool has_buffer_overflow = false;
    bool has_unsafe_string_ops = false;
    
    // Check for path traversal: user input + file operations without validation
    for (const auto& source : sources) {
        for (const auto& sink : sinks) {
            if ((source.type == "user_input" || source.type == "network_input") && 
                sink.type == "file_operation") {
                
                bool has_path_sanitizer = false;
                for (const auto& sanitizer : sanitizers) {
                    if (sanitizer.function_name.find("path") != std::string::npos ||
                        sanitizer.function_name.find("realpath") != std::string::npos) {
                        has_path_sanitizer = true;
                        break;
                    }
                }
                
                if (!has_path_sanitizer) {
                    has_path_traversal = true;
                    break;
                }
            }
        }
        if (has_path_traversal) break;
    }
    
    // Check for buffer overflow: network input + memory operations without bounds checking
    for (const auto& source : sources) {
        for (const auto& sink : sinks) {
            if ((source.type == "network_input" || source.type == "user_input") && 
                sink.type == "memory_operation") {
                
                bool has_buffer_sanitizer = false;
                for (const auto& sanitizer : sanitizers) {
                    if (sanitizer.function_name.find("buffer") != std::string::npos ||
                        sanitizer.function_name.find("length") != std::string::npos) {
                        has_buffer_sanitizer = true;
                        break;
                    }
                }
                
                if (!has_buffer_sanitizer) {
                    has_buffer_overflow = true;
                    break;
                }
            }
        }
        if (has_buffer_overflow) break;
    }
    
    // Determine primary vulnerability
    if (has_path_traversal) {
        has_vulnerability = true;
        primary_cwe = "CWE-22";
        primary_vuln_name = "Path Traversal";
        analysis_details << "Path traversal vulnerability detected: User input flows to file operations without proper path validation, allowing attackers to access files outside intended directories. ";
    } else if (has_buffer_overflow) {
        has_vulnerability = true;
        primary_cwe = "CWE-120";
        primary_vuln_name = "Buffer Overflow";
        analysis_details << "Buffer overflow vulnerability detected: Network input flows to memory operations without bounds checking, enabling potential memory corruption. ";
    } else if (!unsanitized_flows.empty()) {
        has_vulnerability = true;
        primary_cwe = "CWE-20";
        primary_vuln_name = "Improper Input Validation";
        analysis_details << "Input validation vulnerabilities detected: Untrusted data flows to sensitive operations without proper sanitization. ";
    }
    
    // Additional analysis
    if (!has_vulnerability && sources.empty() && sinks.empty()) {
        analysis_details << "No clear data flows identified in the code snippet. If this code calls external APIs, those should be considered potentially unsanitized sources. ";
    } else if (!has_vulnerability) {
        analysis_details << "Based on this analysis, the code appears to have proper input validation and sanitization in place. ";
    }
    
    vulnerability_info.has_vulnerability = has_vulnerability;
    vulnerability_info.cwe_id = primary_cwe;
    vulnerability_info.vulnerability_name = primary_vuln_name;
    vulnerability_info.analysis_details = analysis_details.str();
}

void SecurityAnalyzer::generateSecurityReport() const {
    std::cout << "Here is a data flow analysis of the given code snippet:" << std::endl;
    
    // A. Sources
    std::cout << "A. Sources:" << std::endl;
    if (sources.empty()) {
        std::cout << "1. No explicit sources identified (assuming external API calls are unsanitized)" << std::endl;
    } else {
        for (const auto& source : sources) {
            std::cout << source.id << ". " << source.description << std::endl;
        }
    }
    
    // B. Sinks
    std::cout << "B. Sinks:" << std::endl;
    if (sinks.empty()) {
        std::cout << "1. No explicit sinks identified" << std::endl;
    } else {
        for (const auto& sink : sinks) {
            std::cout << sink.id << ". " << sink.description << std::endl;
        }
    }
    
    // C. Sanitizers
    std::cout << "C. Sanitizers:" << std::endl;
    if (sanitizers.empty()) {
        std::cout << "1. No sanitizers identified" << std::endl;
    } else {
        for (const auto& sanitizer : sanitizers) {
            std::cout << sanitizer.id << ". " << sanitizer.description << std::endl;
        }
    }
    
    // D. Unsanitized Data Flows
    std::cout << "D. Unsanitized Data Flows:" << std::endl;
    if (unsanitized_flows.empty()) {
        std::cout << "1. No unsanitized data flows identified" << std::endl;
    } else {
        for (size_t i = 0; i < unsanitized_flows.size(); ++i) {
            const auto& flow = unsanitized_flows[i];
            std::cout << (i + 1) << ". (source " << flow.source_id << ", sink " << flow.sink_id 
                      << ", " << flow.vulnerability_reason << ")" << std::endl;
        }
    }
    
    // E. Vulnerability analysis
    std::cout << "E. Vulnerability analysis:" << std::endl;
    std::cout << vulnerability_info.analysis_details;
    if (vulnerability_info.analysis_details.empty()) {
        std::cout << "Based on this analysis, no specific vulnerabilities were identified in the given snippet.";
    }
    std::cout << std::endl;
    
    // F. Vulnerability analysis verdict
    std::cout << "F. Vulnerability analysis verdict: $$ vulnerability: " 
              << (vulnerability_info.has_vulnerability ? "YES" : "NO")
              << " | vulnerability type: " << vulnerability_info.cwe_id
              << " | vulnerability name: " << vulnerability_info.vulnerability_name
              << "$$" << std::endl;
}