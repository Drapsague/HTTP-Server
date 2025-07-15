CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SRCDIR = src
INCDIR = include
BINDIR = bin
OBJDIR = obj

# Create directories if they don't exist
$(shell mkdir -p $(OBJDIR) $(BINDIR))

# Security analyzer target
SECURITY_ANALYZER_SOURCES = $(SRCDIR)/securityAnalyzer.cpp $(SRCDIR)/securityAnalyzerMain.cpp
SECURITY_ANALYZER_OBJECTS = $(OBJDIR)/securityAnalyzer.o $(OBJDIR)/securityAnalyzerMain.o
SECURITY_ANALYZER_TARGET = $(BINDIR)/security_analyzer

# Test target
SECURITY_ANALYZER_TEST_SOURCES = $(SRCDIR)/securityAnalyzer.cpp $(SRCDIR)/securityAnalyzerTest.cpp
SECURITY_ANALYZER_TEST_OBJECTS = $(OBJDIR)/securityAnalyzer.o $(OBJDIR)/securityAnalyzerTest.o
SECURITY_ANALYZER_TEST_TARGET = $(BINDIR)/security_analyzer_test

# Default target
all: security_analyzer security_analyzer_test

# Security analyzer executable
security_analyzer: $(SECURITY_ANALYZER_TARGET)

$(SECURITY_ANALYZER_TARGET): $(SECURITY_ANALYZER_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Security analyzer test executable
security_analyzer_test: $(SECURITY_ANALYZER_TEST_TARGET)

$(SECURITY_ANALYZER_TEST_TARGET): $(SECURITY_ANALYZER_TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/securityAnalyzer.o: $(SRCDIR)/securityAnalyzer.cpp $(INCDIR)/securityAnalyzer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/securityAnalyzerMain.o: $(SRCDIR)/securityAnalyzerMain.cpp $(INCDIR)/securityAnalyzer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/securityAnalyzerTest.o: $(SRCDIR)/securityAnalyzerTest.cpp $(INCDIR)/securityAnalyzer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(OBJDIR) $(BINDIR)/security_analyzer $(BINDIR)/security_analyzer_test

# Test target
test: security_analyzer_test
	./$(SECURITY_ANALYZER_TEST_TARGET)

# Run demo
demo: security_analyzer
	./$(SECURITY_ANALYZER_TARGET)

# Analyze existing server code
analyze_server: security_analyzer
	./$(SECURITY_ANALYZER_TARGET) $(SRCDIR)/response.cpp

.PHONY: all clean test demo security_analyzer security_analyzer_test analyze_server