# SPIMI Inverted Index Builder

A C++ implementation of the Single-Pass In-Memory Indexing (SPIMI) algorithm to build an inverted index from a collection of documents.

## Features

- Processes all text files in a specified folder.
- Generates sub-inverted indices based on a maximum term limit.
- Merges sub-indices using a multiway merging algorithm.
- Outputs a final inverted index file (`invIndex.csv`) in CSV format.

## Requirements

- C++17 or newer compiler (e.g., `g++`, `clang++`).
- Standard Library (no external dependencies required).

## Usage

1. **Compilation**:
   ```bash
   g++ -std=c++17 -o spimi *.cpp
