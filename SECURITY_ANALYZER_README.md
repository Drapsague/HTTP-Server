# Security Vulnerability Analyzer for C++ HTTP Server

This repository now includes a comprehensive security vulnerability analyzer that can detect common security flaws in C++ code.

## Overview

The security analyzer implements data flow analysis to identify potential security vulnerabilities in C++ code snippets. It analyzes the flow of data from untrusted sources to sensitive operations (sinks) and identifies when proper sanitization is missing.

## Features

### Supported Vulnerability Detection

1. **Path Traversal (CWE-22)**: Detects when user input flows to file operations without proper path validation
2. **Buffer Overflow (CWE-120)**: Identifies unsafe memory operations with network/user input
3. **Improper Input Validation (CWE-20)**: General category for unsanitized data flows

### Data Flow Analysis Components

- **Sources**: Input sources like network data, user input, file content
- **Sinks**: Sensitive operations like file access, network output, memory operations
- **Sanitizers**: Security controls like input validation, path canonicalization
- **Flows**: Traces data movement from sources to sinks

## Building and Running

### Build the Security Analyzer

```bash
make security_analyzer
```

### Run Tests

```bash
make test
```

### Analyze Code Files

```bash
# Analyze a specific file
./bin/security_analyzer src/response.cpp

# Run demo with built-in examples
./bin/security_analyzer
```

### Run on HTTP Server Code

```bash
make analyze_server
```

## Example Output

The analyzer provides output in the following format:

```
Here is a data flow analysis of the given code snippet:
A. Sources: 
1. Network data received via recv()
2. User input data from buffer

B. Sinks:
1. File system access for reading
2. Memory copy operation

C. Sanitizers:
1. No sanitizers identified

D. Unsanitized Data Flows:
1. (source 2, sink 1, User input flows directly to file operations without path validation, enabling path traversal attacks)

E. Vulnerability analysis:
Path traversal vulnerability detected: User input flows to file operations without proper path validation, allowing attackers to access files outside intended directories.

F. Vulnerability analysis verdict: $$ vulnerability: YES | vulnerability type: CWE-22 | vulnerability name: Path Traversal$$
```

## Vulnerabilities Found in HTTP Server

The analyzer identified several security issues in the original HTTP server code:

1. **Path Traversal (CWE-22)**: In `response.cpp`, user input from HTTP requests is used directly to construct file paths without validation
2. **Buffer Overflow potential**: Unsafe string operations using `memcpy`, `strcpy` without proper bounds checking
3. **Input validation issues**: Network data flows to various operations without sanitization

## Architecture

### Core Classes

- `SecurityAnalyzer`: Main analysis engine
- `DataFlowSource`: Represents input sources
- `DataFlowSink`: Represents output sinks  
- `Sanitizer`: Represents security controls
- `UnsanitizedFlow`: Represents vulnerable data flows
- `VulnerabilityInfo`: Contains vulnerability assessment results

### Key Methods

- `analyzeCodeSnippet()`: Main analysis method
- `identifySources()`: Finds input sources in code
- `identifySinks()`: Finds sensitive operations
- `identifySanitizers()`: Detects security controls
- `analyzeDataFlows()`: Traces data flow paths
- `performVulnerabilityAnalysis()`: Determines vulnerabilities
- `generateSecurityReport()`: Outputs analysis results

## Integration with HTTP Server

The security analyzer is designed as a standalone tool that can be integrated into the build process or used for security auditing of the HTTP server codebase.

## Testing

The test suite validates:
- Source and sink detection accuracy
- Path traversal vulnerability detection
- Buffer overflow detection  
- Secure code analysis (with proper sanitization)
- Report format compliance

Run tests with:
```bash
make test
```

## Future Enhancements

Potential improvements include:
- SQL injection detection
- Cross-site scripting (XSS) detection
- Command injection detection
- More sophisticated data flow tracking
- Integration with CI/CD pipelines
- Configuration file support for custom patterns