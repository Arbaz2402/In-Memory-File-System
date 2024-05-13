#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm> 
using namespace std;

class FileSystem
{
private:
    struct Node
    {
        string name;
        bool is_directory;
        unordered_map<string, Node *> children;

        Node(const string &n, bool is_dir = false) : name(n), is_directory(is_dir) {}
    };

    Node *root;
    Node *current_directory;

public:
    FileSystem()
    {
        root = new Node("/", true);
        current_directory = root;
    }

    ~FileSystem()
    {
        clearFileSystem(root);
    }

    // Create a new directory
    void mkdir(const string &directory_name)
    {
        if (current_directory->children.find(directory_name) == current_directory->children.end())
        {
            Node *new_directory = new Node(directory_name, true);
            current_directory->children[directory_name] = new_directory;
        }
        else
        {
            cout << "Error: Directory '" << directory_name << "' already exists." << endl;
        }
    }

    // Change the current directory
    void cd(const string &path)
    {
        if (path.empty() || path == "/")
        {
            current_directory = root;
            return;
        }

        vector<string> components = splitPath(path);
        Node *target_directory = nullptr;

        if (path[0] == '/')
        {
            target_directory = root;
        }
        else
        {
            target_directory = current_directory;
        }

        for (const auto &component : components)
        {
            if (component == "..")
            {
                target_directory = getParent(target_directory);
            }
            else if (target_directory->children.find(component) != target_directory->children.end())
            {
                target_directory = target_directory->children[component];
            }
            else
            {
                cout << "Error: Directory '" << component << "' not found." << endl;
                return;
            }
        }

        current_directory = target_directory;
    }

    // List the contents of the current or specified directory
    void ls(const string &path = "")
    {
        Node *target_directory;
        if (path.empty())
        {
            target_directory = current_directory;
        }
        else
        {
            target_directory = findDirectory(path);
        }

        if (target_directory)
        {
            for (const auto &entry : target_directory->children)
            {
                cout << entry.first << " ";
            }
            cout << endl;
        }
        else
        {
            cout << "Error: Directory '" << path << "' not found." << endl;
        }
    }

    // Create a new empty file or directory
    void touch(const string &file_name)
    {
        vector<string> path_components = splitPath(file_name);
        string file = path_components.back();
        path_components.pop_back();

        Node *current = current_directory;

        // Traverse to the parent directory
        for (const auto &component : path_components)
        {
            if (component == "..")
            {
                current = getParent(current);
            }
            else if (current->children.find(component) != current->children.end())
            {
                current = current->children[component];
            }
            else
            {
                cout << "Error: Directory '" << component << "' not found." << endl;
                return;
            }
        }

        // Check if the file already exists
        if (current->children.find(file) == current->children.end())
        {
            Node *new_node = new Node(file, false); // Create a file, not a directory
            current->children[file] = new_node;
        }
        else
        {
            cout << "Error: File or directory '" << file << "' already exists." << endl;
        }
    }

    void cat(const string &file_path)
    {
        Node *file_node = findNode(file_path);

        if (!file_node)
        {
            cerr << "Error: File '" << file_path << "' not found." << endl;
            return;
        }

        if (file_node->is_directory)
        {
            cerr << "Error: '" << file_path << "' is a directory, not a file." << endl;
            return;
        }

        cout << "Content of file '" << file_path << "':" << endl;

        // Open the file for reading
        ifstream infile(file_path);
        if (!infile.is_open())
        {
            cerr << "Error: Unable to open file '" << file_path << "' for reading." << endl;
            return;
        }

        // Read and display the content of the file line by line
        string line;
        while (getline(infile, line))
        {
            cout << line << endl;
        }

        // Close the file
        infile.close();
    }

    void echo(const string &file_name, const string &text)
    {
        // Open the file for writing in append mode
        ofstream outfile(file_name, ios::app);
        if (!outfile.is_open())
        {
            // Provide a more detailed error message
            cout << "Error: Unable to open file '" << file_name << "' for writing. ";
            // Check if the file exists
            if (ifstream(file_name))
            {
                cout << "The file exists but cannot be opened for writing." << endl;
            }
            else
            {
                cout << "The file does not exist or the program lacks the necessary permissions." << endl;
            }
            return;
        }

        // Write the text to the file followed by a newline
        outfile << text << endl;

        // Close the file
        outfile.close();

        cout << "Text written to '" << file_name << "' successfully." << endl;
    }

    void mv(const string &source, const string &destination)
    {
        Node *source_node = findNode(source);

        if (!source_node)
        {
            cout << "Error: Source '" << source << "' not found." << endl;
            return;
        }

        Node *destination_node = findDirectory(destination);

        if (!destination_node)
        {
            cout << "Error: Destination directory '" << destination << "' not found." << endl;
            return;
        }

        // Check if the destination already has a node with the same name
        if (destination_node->children.find(source_node->name) != destination_node->children.end())
        {
            cout << "Error: Destination '" << destination << "' already contains a node with the same name." << endl;
            return;
        }

        // Remove the source node from its parent directory
        Node *source_parent = getParent(source_node);
        source_parent->children.erase(source_node->name);

        // Add the source node to the destination directory
        destination_node->children[source_node->name] = source_node;

        cout << source << " moved to " << destination << endl;
    }

    // Copy a file or directory to another location
    void cp(const string &source, const string &destination)
    {
        Node *source_node = findDirectoryOrFile(source);
        if (source_node)
        {
            Node *destination_node = findDirectory(destination);

            if (!destination_node)
            {
                cout << "Error: Destination directory '" << destination << "' not found." << endl;
                return;
            }

            // Check if the destination already has a node with the same name
            if (destination_node->children.find(source_node->name) != destination_node->children.end())
            {
                cout << "Error: Destination '" << destination << "' already contains a node with the same name." << endl;
                return;
            }

            // Copy the source node to the destination directory
            Node *copy_node = copyNode(source_node);
            destination_node->children[source_node->name] = copy_node;

            cout << source << " copied to " << destination << endl;
        }
        else
        {
            cout << "Error: Source '" << source << "' not found." << endl;
        }
    }

    Node *findNode(const string &path)
    {
        // Start searching from the current directory
        return findNodeRecursive(current_directory, path);
    }

    //  function to delete a node and its children
    void deleteNode(Node *node)
    {
        if (node->is_directory)
        {
            // Delete all children recursively
            for (auto &entry : node->children)
            {
                deleteNode(entry.second);
            }
        }

        delete node;
    }

    // Function to remove a file or directory
    void rm(const std::string &name)
    {
        Node *target = findNode(name);

        if (!target)
        {
            std::cout << "Error: '" << name << "' not found." << std::endl;
            return;
        }

        Node *parent = getParent(target);

        if (parent)
        {
            // Clean up memory
            deleteNode(target);

            // Remove the target from the parent's children
            auto it = parent->children.find(name);
            if (it != parent->children.end())
            {
                parent->children.erase(it);
                std::cout << name << " removed successfully." << std::endl;
            }
            else
            {
                std::cout << "Error: Could not remove '" << name << "' from parent." << std::endl;
            }
        }
        else
        {
            std::cout << "Error: Cannot remove root directory." << std::endl;
        }
    }

    // Function to get the current directory name
    string getCurrentDirectory() const
    {
        return current_directory->name;
    }

    // Function to split a string into tokens
    vector<string> splitString(const string &input) const
    {
        vector<string> tokens;
        istringstream iss(input);
        string token;
        while (iss >> token)
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    // Function to save the state of the file system
    void saveState(const string &filepath)
    {
        ofstream file(filepath);
        saveStateHelper(root, file);
        file.close();
    }

    void saveStateHelper(Node *node, ofstream &file)
    {
        file << node->name << " " << (node->is_directory ? "directory" : "file") << endl;

        for (const auto &entry : node->children)
        {
            saveStateHelper(entry.second, file);
        }
    }
    void loadState(const string &filepath)
    {
        clearFileSystem(root);
        root = new Node("/", true);
        current_directory = root;

        ifstream file(filepath);
        if (file.is_open())
        {
            loadStateHelper(root, file);
            file.close();
        }
        else
        {
            cerr << "Error: Unable to open file " << filepath << endl;
        }
    }

    void loadStateHelper(Node *node, ifstream &file)
    {
        string name, type;
        if (!(file >> name >> type))
        {
            cerr << "Error: Unable to read from file." << endl;
            return;
        }

        Node *new_node = new Node(name, type == "directory");

        if (type == "directory")
        {
            while (file >> name >> type)
            {
                if (name == "]" || type == "]")
                {
                    break;
                }
                loadStateHelper(new_node, file);
            }
        }

        node->children[name] = new_node;
    }

private:
    //  Function to find a directory by path
    Node *findDirectory(const string &path)
    {
        vector<string> components = splitPath(path);
        Node *current = current_directory;

        for (const auto &component : components)
        {
            if (component == "..")
            {
                current = getParent(current);
            }
            else if (current->children.find(component) != current->children.end())
            {
                current = current->children[component];
            }
            else
            {
                return nullptr;
            }
        }

        return current;
    }

    //  function to find a directory or file by name
    Node *findDirectoryOrFile(const string &name)
    {
        if (current_directory->children.find(name) != current_directory->children.end())
        {
            return current_directory->children[name];
        }
        else
        {
            return nullptr;
        }
    }

    //  function to get the parent directory of a node
    Node *getParent(Node *node)
    {
        if (node == root)
        {
            return root;
        }

        for (auto &entry : root->children)
        {
            if (entry.second == node)
            {
                return root;
            }
            else if (entry.second->is_directory && entry.second->children.find(node->name) != entry.second->children.end())
            {
                return entry.second->children[node->name];
            }
        }

        return nullptr;
    }

    //  function to split a path into components
    vector<string> splitPath(const string &path)
    {
        vector<string> result;
        size_t start = 0, end;

        while ((end = path.find('/', start)) != string::npos)
        {
            result.push_back(path.substr(start, end - start));
            start = end + 1;
        }

        result.push_back(path.substr(start));
        return result;
    }

    //  function to clear the file system recursively
    void clearFileSystem(Node *node)
    {
        for (auto &entry : node->children)
        {
            clearFileSystem(entry.second);
        }
        delete node;
    }

    //  function to copy a node and its children
    Node *copyNode(const Node *source)
    {
        Node *copy = new Node(source->name, source->is_directory);
        if (source->is_directory)
        {
            for (const auto &entry : source->children)
            {
                copy->children[entry.first] = copyNode(entry.second);
            }
        }
        return copy;
    }

    //  function to the private section of the FileSystem class
    Node *findNodeRecursive(Node *current, const string &path)
    {
        if (path.empty() || path == ".")
        {
            return current;
        }

        size_t pos = path.find('/');
        string next_component = (pos == string::npos) ? path : path.substr(0, pos);

        if (current->children.find(next_component) != current->children.end())
        {
            Node *next_node = current->children[next_component];
            if (pos == string::npos)
            {
                return next_node;
            }
            else
            {
                return findNodeRecursive(next_node, path.substr(pos + 1));
            }
        }
        else
        {
            return nullptr; // Component not found
        }
    }
};

int main()
{
    FileSystem fileSystem;

    while (true)
    {
        cout << "Current Directory: " << fileSystem.getCurrentDirectory() << " $ ";
        string command;
        getline(cin, command);

        if (command.empty())
        {
            continue;
        }

        vector<string> tokens = fileSystem.splitString(command);

        if (tokens.size() >= 1)
        {
            string operation = tokens[0];
            tokens.erase(tokens.begin());

            if (operation == "exit")
            {
                break;
            }
            else if (operation == "ls" && tokens.empty())
            {
                fileSystem.ls();
            }
            else if (operation == "ls" && tokens.size() == 1)
            {
                fileSystem.ls(tokens[0]);
            }
            else if (operation == "cd" && tokens.size() == 1)
            {
                fileSystem.cd(tokens[0]);
            }
            else if (operation == "mkdir" && tokens.size() == 1)
            {
                fileSystem.mkdir(tokens[0]);
            }
            else if (operation == "touch" && tokens.size() == 1)
            {
                fileSystem.touch(tokens[0]);
            }
            else if (operation == "mv" && tokens.size() == 2)
            {
                fileSystem.mv(tokens[0], tokens[1]);
            }
            else if (operation == "cp" && tokens.size() == 2)
            {
                fileSystem.cp(tokens[0], tokens[1]);
            }
            else if (operation == "rm" && tokens.size() == 1)
            {
                fileSystem.rm(tokens[0]);
            }
            else if (operation == "echo" && tokens.size() >= 3 && tokens[tokens.size() - 2] == ">")
            {
                // Extract file name from tokens
                string file_name = tokens.back();
                // Reconstruct text to be echoed
                string text;
                for (size_t i = 0; i < tokens.size() - 2; ++i)
                {
                    if (tokens[i].front() == '\'' && tokens[i].back() == '\'')
                    {
                        text += tokens[i].substr(1, tokens[i].size() - 2) + " ";
                    }
                    else
                    {
                        text += tokens[i] + " ";
                    }
                }
                fileSystem.echo(file_name, text);
            }

            else if (operation == "cat" && tokens.size() == 1)
            {
                fileSystem.cat(tokens[0]);
            }
            else if (operation == "save" && tokens.empty())
            {
                fileSystem.saveState("file_system_state.txt");
                cout << "State saved to file_system_state.txt" << endl;
            }
            else if (operation == "load" && tokens.empty())
            {
                fileSystem.loadState("file_system_state.txt");
                cout << "State loaded from file_system_state.txt" << endl;
            }
            else
            {
                cout << "Error: Invalid command." << endl;
            }
        }
        else
        {
            cout << "Error: Invalid command." << endl;
        }
    }
    return 0;
}
