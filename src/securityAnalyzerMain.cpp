#include "../include/securityAnalyzer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::string readFileContent(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void analyzeCodeSnippet(const std::string& code_snippet) {
    SecurityAnalyzer analyzer;
    analyzer.analyzeCodeSnippet(code_snippet);
    analyzer.generateSecurityReport();
}

void demonstrateVulnerableCode() {
    std::cout << "\n=== Analyzing vulnerable HTTP server code ===" << std::endl;
    
    // Example of vulnerable code from the HTTP server
    std::string vulnerable_code = R"(
void Response::recv_request() {
    std::memset(m_recvBuffer.get(), 0, m_recvBuffer_size);
    ssize_t rec = recv(connection_->m_clientSocket, m_recvBuffer.get(), m_recvBuffer_size, 0);
    if (rec > 0) {
        m_recvBuffer.get()[rec] = '\0';
    }
}

std::unique_ptr<char[]> Response::get_file(char* header_ptr) {
    std::ifstream file(header_ptr + 1, std::ios::binary);
    if (!file) {
        return nullptr;
    }
    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::unique_ptr<char[]> file_ptr = std::make_unique<char[]>(file_size + 1);
    file.read(file_ptr.get(), file_size);
    return file_ptr;
}

std::unique_ptr<char[]> Response::get_header_file() {
    char* without_get = m_recvBuffer.get() + 4;
    char* f_line = strchr(without_get, ' ');
    size_t file_s = strlen(without_get) - strlen(f_line);
    
    std::unique_ptr<char[]> header = new char(file_s + 8);
    memcpy(header.get(), "/public", 7);
    memcpy(header.get() + 7, without_get, file_s);
    
    return header;
}
    )";
    
    analyzeCodeSnippet(vulnerable_code);
}

void demonstrateSecureCode() {
    std::cout << "\n=== Analyzing secure code with proper validation ===" << std::endl;
    
    std::string secure_code = R"(
#include <limits.h>

bool validatePath(const char* path) {
    if (!path) return false;
    
    // Check for directory traversal
    if (strstr(path, "..") != nullptr) {
        return false;
    }
    
    // Validate path length
    if (strlen(path) > PATH_MAX) {
        return false;
    }
    
    return true;
}

std::unique_ptr<char[]> Response::get_file_secure(char* header_ptr) {
    if (!validatePath(header_ptr)) {
        return nullptr;
    }
    
    char canonical_path[PATH_MAX];
    if (realpath(header_ptr + 1, canonical_path) == nullptr) {
        return nullptr;
    }
    
    std::ifstream file(canonical_path, std::ios::binary);
    if (!file) {
        return nullptr;
    }
    
    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    if (file_size > MAX_FILE_SIZE) {
        return nullptr;
    }
    
    file.seekg(0, std::ios::beg);
    std::unique_ptr<char[]> file_ptr = std::make_unique<char[]>(file_size + 1);
    file.read(file_ptr.get(), file_size);
    
    return file_ptr;
}
    )";
    
    analyzeCodeSnippet(secure_code);
}

int main(int argc, char* argv[]) {
    std::cout << "Security Vulnerability Analyzer for C++ Code" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    if (argc > 1) {
        // Analyze file provided as command line argument
        std::string filename = argv[1];
        std::string code_content = readFileContent(filename);
        
        if (!code_content.empty()) {
            std::cout << "\nAnalyzing file: " << filename << std::endl;
            analyzeCodeSnippet(code_content);
        }
    } else {
        // Demonstrate with built-in examples
        demonstrateVulnerableCode();
        demonstrateSecureCode();
        
        std::cout << "\nTo analyze a specific file, run: " << argv[0] << " <filename>" << std::endl;
    }
    
    return 0;
}