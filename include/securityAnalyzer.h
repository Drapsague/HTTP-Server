#ifndef SECURITY_ANALYZER_H
#define SECURITY_ANALYZER_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>

struct DataFlowSource {
    int id;
    std::string description;
    std::string location;
    std::string type; // "user_input", "file_input", "network_input", etc.
};

struct DataFlowSink {
    int id;
    std::string description;
    std::string location;
    std::string type; // "file_operation", "network_output", "system_call", etc.
};

struct Sanitizer {
    int id;
    std::string description;
    std::string location;
    std::string function_name;
};

struct UnsanitizedFlow {
    int source_id;
    int sink_id;
    std::string vulnerability_reason;
    std::string flow_path;
};

struct VulnerabilityInfo {
    bool has_vulnerability;
    std::string cwe_id;
    std::string vulnerability_name;
    std::string analysis_details;
};

class SecurityAnalyzer {
private:
    std::vector<DataFlowSource> sources;
    std::vector<DataFlowSink> sinks;
    std::vector<Sanitizer> sanitizers;
    std::vector<UnsanitizedFlow> unsanitized_flows;
    VulnerabilityInfo vulnerability_info;

    // Helper methods for analysis
    void identifySources(const std::string& code_snippet);
    void identifySinks(const std::string& code_snippet);
    void identifySanitizers(const std::string& code_snippet);
    void analyzeDataFlows(const std::string& code_snippet);
    void performVulnerabilityAnalysis();
    
    // Pattern matching helpers
    bool containsPattern(const std::string& code, const std::string& pattern);
    std::vector<size_t> findPatternOccurrences(const std::string& code, const std::string& pattern);
    bool isPathTraversalVulnerable(const std::string& code);
    bool isBufferOverflowVulnerable(const std::string& code);
    bool hasUnsafeStringOperations(const std::string& code);

public:
    SecurityAnalyzer();
    ~SecurityAnalyzer() = default;

    // Main analysis method
    void analyzeCodeSnippet(const std::string& code_snippet);
    
    // Output method in required format
    void generateSecurityReport() const;
    
    // Getter methods for testing
    const std::vector<DataFlowSource>& getSources() const { return sources; }
    const std::vector<DataFlowSink>& getSinks() const { return sinks; }
    const std::vector<Sanitizer>& getSanitizers() const { return sanitizers; }
    const std::vector<UnsanitizedFlow>& getUnsanitizedFlows() const { return unsanitized_flows; }
    const VulnerabilityInfo& getVulnerabilityInfo() const { return vulnerability_info; }
    
    // Clear previous analysis
    void reset();
};

#endif