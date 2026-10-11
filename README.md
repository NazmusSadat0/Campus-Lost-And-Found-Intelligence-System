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
### 4. Run the application

On Linux or macOS:

make run

Alternatively:
``` bash
./campus_lost_found
```

On Windows, the executable may be named campus_lost_found.exe, depending on your build environment. Run it from your terminal using the appropriate executable command.

This command compiles the source files and links the object files into the `campus_lost_found` executable.
## Using the Application

After launching the program, a menu appears with the following options:

text
CAMPUS LOST & FOUND INTELLIGENCE SYSTEM

1. Report Lost Item
2. Report Found Item
3. Display Reports
4. Search Reports
5. Update Report
6. Delete Report
7. Find Possible Matches
8. Record Item Return
0. Save and Exit


Enter the corresponding option and follow the prompts.

When adding a report, provide the requested item details, including its name, category, colour, location, date, description, reporter information, and contact information.

To identify possible matches, select option 7 and enter the ID of an existing report. The system calculates similarity scores and displays qualifying matches.

Select option 0 to save the latest changes and exit.

## Data Storage and Reliability

The application stores records locally in items.dat.

The storage module includes:

- A file signature to identify the expected data format.
- Validation of record counts and stored records.
- Duplicate-ID checks.
- Temporary-file writing before replacing the existing data file.
- Error handling for common file-reading and file-writing failures.

*Important:* The binary data format is intended for compatible builds using the same structure layout and platform assumptions. It is not a portable interchange format between arbitrary compilers or platforms. Keep a backup of important records before changing the data format or modifying the structure definitions.

The application does not require a remote database or internet connection for its core functionality.

## Build Commands

| Command | Purpose |
|---|---|
| make | Compile and link the application |
| make run | Build if necessary and launch the application |
| make clean | Remove object files and the generated executable |

If make is unavailable, compile the source files directly with GCC:

bash
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 \
    main.c items.c matching.c storage.c \
    -o campus_lost_found


Then run the generated executable using the command appropriate for your operating system.

## Concepts Demonstrated

This project demonstrates practical applications of fundamental C programming concepts:

- Structures (struct) for representing item records.
- Functions and modular programming.
- Arrays and string processing.
- Conditional statements, loops, and menu-driven interaction.
- Searching, sorting, and score-based matching.
- File handling and binary data storage.
- Input validation and error handling.
- Header files and multi-file compilation.
- Makefiles and basic build automation.

## Limitations

- The application runs in a terminal rather than a graphical interface.
- Records are stored locally and are not automatically shared across computers.
- Match scores depend on the information entered by users.
- The system does not independently verify item ownership.
- The application is designed for a limited number of records, with a maximum capacity defined in the source code.
- The current implementation does not provide a multi-user web interface or centralized database.

## Future Improvements

Potential extensions include:

- A graphical user interface or web-based frontend.
- Centralized database storage for campus-wide access.
- User authentication and administrative roles.
- More advanced matching using normalized text and additional item attributes.
- Notifications when a likely match is found.
- Report timestamps and audit history.
- Image uploads for item identification.
- Improved portability through a versioned, platform-independent storage format.

## Academic Purpose

This project was developed as a practical exercise in C programming, data structures, modular software development, searching, sorting, file handling, and error management. It models a campus lost-and-found workflow and demonstrates how fundamental programming concepts can be combined to solve a practical problem.

## License

No license has been specified yet. Unless a license is added to this repository, others should not assume that they have permission to redistribute or modify the project.

---

*Project:* Campus Lost & Found Intelligence System  
*Language:* C  
*Application Type:* Menu-driven terminal application  
*Storage:* Local binary file
