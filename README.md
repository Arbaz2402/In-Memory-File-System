# In-Memory-File-System

1. Introduction:

This document presents the implementation details of an in-memory file system designed to support various functionalities such as directory manipulation, file operations, navigation, and search. The file system aims to replicate the behavior of a typical terminal interface, offering users familiar commands and interactions.

2. Implementation:

File System Structure:

The file system is organized in a tree-like hierarchy consisting of directories and files.
Each directory is represented by a node containing its name and a list of child nodes (directories or files).
Files are represented as leaf nodes with their names and contents stored within the node.
Data Structures Used:

Node Structure: Represents a directory or file node in the file system. Contains properties like name, type, contents (for files), and a list of child nodes.
Directory Stack: Utilized for directory navigation and path resolution, a stack data structure keeps track of the current directory and facilitates traversal operations such as cd and ls.

Design Decisions:

•	Object-Oriented Approach: The implementation follows an object-oriented design, with classes for the file system, directory nodes, and file nodes, promoting modularity and extensibility.

•	Error Handling: Robust error handling mechanisms are incorporated to gracefully manage invalid inputs, file not found errors, permission issues, and other exceptional conditions.

•	Path Resolution: Support for navigating to the parent directory (..), root directory (/), and absolute paths enables flexible navigation options akin to a standard terminal.

•	Persistence (Bonus): Additional functionality is implemented to support saving and loading the state of the file system from persistent memory, allowing users to resume operations from where they left off in subsequent sessions.


3.Usage Instructions:

•	Compile the provided C++ code in your preferred environment.
•	Run the executable generated after compilation.
•	Follow the command-line prompts to input commands and interact with the file system.
•	Refer to the syntax and examples provided below for each supported functionality.

4.Syntax and Examples:


•	mkdir: Create a new directory.
Syntax: mkdir <directory_name>
Example: mkdir documents

•	cd: Change the current directory.
Syntax: cd <path>
Examples:
cd /
cd ..
cd documents
cd /home/user/documents

ls: List the contents of the current directory or a specified directory.
Syntax: ls [<path>]
Examples:
ls
ls documents
ls /home/user/documents

•	cat: Display the contents of a file.
Syntax: cat <file_path>
Example: cat file.txt

•	touch: Create a new empty file.
Syntax: touch <file_name>
Example: touch file.txt

•	echo: Write text to a file.
Syntax: echo '<text>' > <file_name>
Example: echo 'Hello, world!' > greeting.txt

•	mv: Move a file or directory to another location.
Syntax: mv <source> <destination>
Example: mv /data/ice_cream/cassata.txt ./data/boring/ice_cream/mississippimudpie/

•	cp: Copy a file or directory to another location.
Syntax: cp <source> <destination>
Example: cp /data/ice_cream/cassata.txt .

•	rm: Remove a file or directory.
Syntax: rm <name>
Example: rm file.txt


