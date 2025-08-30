
# UDP/TCP Mailbox System

**Authors:** Drenou Grégoire & Hajji Sammy  
**School:** INSA Toulouse – 3IMACS - 3A AE

---

## Project Description

This project implements a distributed mailbox system in C using the socket API (TCP/UDP).

- **Emitters (E):** Send messages ("letters") to recipients.
- **Recipients (R):** Retrieve their letters.
- **Mailbox Server (BAL):** Acts as an intermediary, storing messages until collected.
---

## Installation

```bash
make
```
This will generate the executable `tsock`.

---

## Usage Examples

### Start the Mailbox Server
```bash
./tsock -b 8000
```

### Send Letters (Emitter)
```bash
./tsock -e2 -n3 -l45 localhost 8000
```
Sends 3 letters of 45 bytes from emitter #2 to the server on port 8000.

### Receive Letters (Receiver)
```bash
./tsock -r2 -l45 localhost 8000
```
Retrieves all stored letters for receiver #2 from the server.

---

## Command-Line Options

- `-r` : Run as receiver, waiting on a given port
- `-e` : Run as emitter, sending to a host and port
- `-b port` : Run as a Mailbox server on port
- `-u` : Use UDP instead of TCP (default is TCP)
- `-l ##` : Set message length (default: 30 bytes)
- `-n ##` : Set number of messages (default: 10 for senders, infinite for receivers)

---

## Features

- Socket-based communication (TCP/UDP)
- Mailbox system with multiple emitters and receivers
- Configurable message size, number of messages, and recipients
- Properly formatted displays for both sent and received messages

---

## Design Choices

- Message storage uses FILO (First-In-Last-Out) 
- All messages from an emitter are stored (no overwriting)
- Code refactored for readability (external functions, header file)
- Makefile provided for easy compilation and execution

---

## Testing

You can test the program by modifying:
- The role (emitter/receiver)
- The port number
- The address (localhost or remote host)
- The number of messages (`-n`)
- The message length (`-l`)

**Default values:**
- `n = 10` messages
- `l = 30` bytes per message

---
