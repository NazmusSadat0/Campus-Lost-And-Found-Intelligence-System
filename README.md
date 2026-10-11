# Campus Lost & Found Intelligence System

A menu-driven, terminal-based application developed in **C** to manage lost and found items on a campus. The system allows users to report lost or found belongings, search existing reports, update or delete records, identify possible matches, and record returned items. It uses structured programming, modular source files, and file handling to maintain records between sessions.

## Features

- **Report Lost Items:** Record details about missing belongings.
- **Report Found Items:** Add information about items found on campus.
- **Display Reports:** View existing lost and found records.
- **Search Reports:** Search for relevant item records.
- **Update Reports:** Modify existing report information.
- **Delete Reports:** Remove unwanted or incorrect reports.
- **Intelligent Match Scoring:** Compare lost and found reports using category, item name, colour, and location.
- **Rank Possible Matches:** Display qualifying matches in descending score order.
- **Record Item Returns:** Update reports when an item is returned.
- **Persistent Data Storage:** Save records to a local data file and load them when the application starts.
- **Input and Data Validation:** Validate records and check saved data before accepting it.
- **Modular Architecture:** Separate program logic, matching algorithms, and storage operations into different source files.

## Technology Stack

- **Programming Language:** C
- **Compiler:** GCC
- **C Standard:** ISO C11
- **Build System:** GNU Make
- **Data Storage:** Local binary file (`items.dat`)
- **Development Tools:** Visual Studio Code or any C-compatible editor and terminal
- **Version Control:** Git and GitHub

## Project Structure

```text
Campus-Lost-And-Found-Intelligence-System/
│
├── main.c                 # Main menu and program execution
├── items.c                # Item operations and input validation
├── matching.c             # Match scoring and match ranking
├── storage.c              # Saving and loading records
├── items.h                # Structures, constants and declarations
├── Makefile               # Build, run and clean commands
├── items.dat              # Persistent application data
└── README.md              # Project documentation
```

**Note:** `items.dat` is generated or updated by the application as records are saved. If you distribute the project with this file, it may contain existing records.

## How the Matching System Works

The matching module compares an open lost-item report with open found-item reports of the opposite type.

| Matching criterion | Score |
|---|---:|
| Same category | 40 points |
| Same item name | 30 points |
| Same colour | 15 points |
| Same location | 15 points |
| **Maximum score** | **100 points** |

A report qualifies as a possible match when its score is at least **55/100**. Qualifying matches are sorted from highest to lowest score.

The score indicates similarity between report details. It is **not a probability of ownership or proof that an item belongs to someone**. Users should verify ownership before returning an item.

## Requirements

Before building the application, ensure that you have:

- GCC or another compatible C compiler.
- GNU Make for using the supplied Makefile.
- A terminal or command-line interface.

On Windows, you can use a GCC environment such as MinGW-w64 with a compatible Make installation. On Linux, install GCC and Make using your distribution's package manager.

## Installation and Setup

### 1. Clone the repository

```bash
git clone <YOUR_GITHUB_REPOSITORY_URL>
```

### 2. Navigate to the project directory

```bash
cd Campus-Lost-And-Found-Intelligence-System
```

Replace the directory name with your actual cloned folder name if it differs.

### 3. Compile the project

```bash
make
```

This command compiles the source files and links the object files into the `campus_lost_found` executable.
