# Order Management System
Technologies: (https://skillicons.dev/icons?i=expressjs,docker,c,cpp)](https://skillicons.dev)
This project is a simple order management system built using the **Crow C++ web framework**. It provides RESTful API endpoints for managing orders, positions, and broadcasting messages to connected WebSocket clients.

## Features

- **REST API**:
  - Get the order book.
  - Place a new order.
  - Modify an existing order.
  - Cancel an order.
  - Get positions.
- **WebSocket Support**:
  - Broadcast messages to all connected clients.
  - Handle WebSocket connections for real-time updates.
- **Multithreaded**:
  - Supports concurrent requests using multithreading.

---

## Prerequisites

Before running the project, ensure you have the following installed:

- **C++ Compiler** (e.g., `g++` or `clang++`)
- **Crow C++ Framework** (included in the project)
- **CMake** (for building the project)

---

## Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-username/order-management-system.git
   cd order-management-system
