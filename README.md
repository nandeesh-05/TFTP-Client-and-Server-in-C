# TFTP Client and Server in C

## 📌 Description

Implemented a **TFTP (Trivial File Transfer Protocol) Client–Server** application in C using **UDP sockets**. The project demonstrates reliable file transfer over a connectionless protocol by implementing application-level acknowledgments, block numbering, and retransmission logic, similar to real-world TFTP systems.

## 🎯 Key Features

* UDP-based client–server communication
* Supports **Read (RRQ)** and **Write (WRQ)** operations
* Reliable data transfer using ACK mechanism
* Block-based data transmission
* Supports multiple transfer modes:

  * **Normal** (512 bytes/block – default)
  * **Octet** (1 byte at a time)
  * **Netascii** (\n → \r\n conversion)

## 🛠️ Technologies Used

* Programming Language: C (Advanced C)
* Networking: UDP Sockets
* OS: Linux
* Concepts: Client–Server Architecture, Protocol Design
* Build Tool: Makefile

## ▶️ How to Compile and Run

Using Makefile:

```bash
make
./tftp_server
./tftp_client
```

## 📌 System Calls Used

* File Operations: `open()`, `close()`, `read()`, `write()`, `lseek()`
* Networking: `socket()`, `bind()`, `sendto()`, `recvfrom()`

## ⚙️ Working Principle

TFTP operates over UDP, which does not guarantee delivery or order. To ensure reliability, the application implements an **ACK-based mechanism**. Each data packet is sent with a block number, and the receiver responds with an ACK confirming successful receipt. If an ACK is missing or mismatched, the sender retransmits the packet.

## 📦 Packet Structure

The protocol uses opcodes to identify packet types:

* RRQ (Read Request)
* WRQ (Write Request)
* DATA
* ACK
* ERROR

A `union` is used inside the packet structure to efficiently manage memory for different packet formats.

## 🚧 Challenges Faced

* Ensuring reliable transfer over UDP using ACKs
* Synchronizing block numbers between client and server
* Implementing Netascii conversion within buffer limits
* Correct usage of unions for different packet types
* Debugging cross-system network communication

## 📚 Key Learnings

* Deep understanding of **client–server architecture**
* Practical experience with **Linux system calls**
* Protocol and packet structure design
* Handling real-time data transfer issues

## 🌍 Real-World Applications

* Embedded firmware transfer
* Bootloader file updates
* Lightweight network file sharing
* System recovery tools
* Network configuration distribution

## 🧠 Skills Demonstrated

* C Programming
* Socket Programming (UDP)
* Linux Internals
* Protocol Design
* Reliable Data Transfer
* Debugging & Problem Solving
