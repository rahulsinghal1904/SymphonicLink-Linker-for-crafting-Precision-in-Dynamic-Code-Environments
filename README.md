# 🛠️  Linker & Kernel Process Scheduler

---

## 📚 Overview

This repository presents a modular **Kernel Development Toolkit**, combining:

- **SymphonicLink** — a dynamic, precision-oriented linker for efficiently crafting executable modules in distributed systems.
- **ProcessSched** — a priority-based, multilevel queue **process scheduler** supporting efficient CPU scheduling, memory management, and concurrent process execution.

Together, they provide a foundational framework for building and managing **lightweight, high-performance kernel-level operations**.

---

## 🚀 Projects Included

### 1.  Dynamic Linker

- **Description**:  
  Developed a two-pass linker system in C++, enabling modular compilation and dynamic linking for complex distributed codebases.
  
- **Key Features**:
  - External symbol resolution.
  - Relative addressing and module relocation.
  - Efficient memory usage (~30% reduction).
  - Optimized for dynamic environments with shifting code dependencies.

- **Technical Highlights**:
  - Implemented in C++ with custom memory and symbol management.
  - Emphasis on modular code generation for kernel modules.

---

### 2. ProcessSched – Process Scheduler

- **Description**:  
  A Linux-kernel inspired **priority-based multilevel scheduler** and **virtual memory manager**, enabling efficient management of 50+ concurrent processes.

- **Key Features**:
  - Priority queue scheduling.
  - Context switching optimization (30% reduction).
  - Virtual memory management via paging and segmentation.
  - Demand paging for memory efficiency (25% improved allocation).
  - Custom system calls for process management.

- **Technical Highlights**:
  - Developed using C++ with Linux system call interfacing.
  - Focused on concurrency, scheduling fairness, and CPU utilization optimization.

---

## 🛠️ Build Instructions

### Requirements
- `g++` (C++17 or later)
- Linux/macOS environment
- `Make`

### Building

```bash
make linker
./linker input_module1.txt input_module2.txt
