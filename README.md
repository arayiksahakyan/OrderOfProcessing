# Order Of Processing

# Header Dependency Resolver

## Introduction

This C++ project is designed to analyze and resolve header file dependencies within a specified directory. It effectively generates an order of compilation for .h files, ensuring that all dependencies are correctly managed. This is particularly useful in large projects where managing header file dependencies manually can be complex and error-prone.

## Prerequisites

- C++ Compiler (e.g., GCC, Clang)
- Standard C++ Library
- Linux-based OS (for dirent.h)

## Building the Project

To build this project, navigate to the project directory and use the following command:

g++ -o DependencyResolver main.cpp


This will compile the code and create an executable named DependencyResolver.

## Running the Program

After building the project, you can run the program by executing:

./DependencyResolver


The program expects a directory path as input. It will scan this directory for .h files and determine an appropriate order for their inclusion based on their dependencies.

## How it Works

- FilenameMatcher: This class scans a given directory and creates mappings between filenames and numeric identifiers.

- DependencyProvider: This class parses each file to extract its dependencies and forms a dependency graph.

- OrderProvider: Using the dependency graph, this class performs a topological sort to provide a valid order for header inclusion.

- OrderProvidingEngine: This is the main engine that ties together all components, reads the directory path from std::cin, and writes the order to std::cout.

## Example

Input:
/path/to/headers


Output:
header1.h
header2.h
...
