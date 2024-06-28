// ProjectTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;

/// 
///               GLOBALS HERE
/// 

bool _secureMode = false; // Enables (true) secure mode, keeping the file path restricted to only whatever is hard coded. 
                         // While false, allows the use of a custom file path. 
string _filePath = "CS 300 ABCU_Advising_Program_Input.csv"; // The filepath to the courses txt

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct Course
{                            // Contains all relevant course info
    string courseNumber;     // CSCI300 as an example
    string name;             // Computer Science 300 for example
    vector<string> prereqs;  // All of the prereqs will be kept here. [0..*].

    Course(string courseID, string courseName, vector<string> prerequisites) : courseNumber(courseID), name(courseName), prereqs(prerequisites) {};

    Course(string courseID, string courseName) : courseNumber(courseID), name(courseName) { // Default constructor, without prereqs given
        prereqs.clear();                                            // TODO: These constructors were made with an older implementation method
    };                                                              // of the BST constructor in mind. They should be re-assessed
                                                                    // at some point.
    Course(string courseId) : courseNumber(courseId) {    
        name.clear();                       
        prereqs.clear();
    };

    Course() {
        courseNumber.clear();
        name.clear();
        prereqs.clear();
    };
};

struct BinarySearchTree
{
    struct Node // This struct is based upon nodes; Each node in a BST has a left (less than the node) path and a right (greater than) path.
    {
        Node* left;
        Node* right;
        Course* course;

        Node(Course* course) : course(course), left(nullptr), right(nullptr) {};
    };

    Node* root; // The root node is the first Node in the tree. All subsequent nodes are GT or LT this one's value.

    vector<Course*> courses = {}; // This vector will keep track of all courses, for running the invalidator.

    BinarySearchTree() : root(nullptr) {};

    void deleteCourse(const string& courseNumber) { // Call for the deletion of a course by its unique ID.
        root = deleteNode(root, courseNumber);
    }

    Node* deleteNode(Node* root, const string& courseNumber) {
        if (root == nullptr) return root; // Easiest scenario: Root does not exist; Return the nullptr. 

        if (courseNumber < root->course->courseNumber) { // If less than the current node, move left.
            root->left = deleteNode(root->left, courseNumber);
        }
         
        else if (courseNumber > root->course->courseNumber) { // If more, move right.
            root->right = deleteNode(root->right, courseNumber);
        }

        else {
            if (root->left == nullptr) {
                Node* temp = root->right;
                delete root;
                return temp;
            }
            else if (root->right == nullptr) {
                Node* temp = root->left;
                delete root;
                return temp;
            }

            Node* temp = minValueNode(root->right);

            root->course = temp->course;

            root->right = deleteNode(root->right, temp->course->courseNumber);
        }
        return root;
    }

    Node* minValueNode(Node* node) {
        Node* current = node;

        while (current && current->left != nullptr) {
            current = current->left;
        }

        return current;
    }

    void insert(Course* course) {
        root = insert(root, course);
    }

    Node* insert(Node* node, Course* course) {
        if (node == nullptr) {
            courses.push_back(course);
            return new Node(course);
        }
        if (course->courseNumber < node->course->courseNumber) {
            node->left = insert(node->left, course);
        }
        else if (course->courseNumber > node->course->courseNumber) {
            node->right = insert(node->right, course);
        }
        return node;
    }

    void printCourse(string courseId) {
        Node* node = search(courseId);
        if (node != nullptr) {
            cout << "Course ID: " << node->course->courseNumber << " | ";
            cout << "Course Name: " << node->course->name;
            if (node->course->prereqs.empty() == false) {
                cout << " | Prerequisites: ";
                int i = 0;
                for (const auto& prereq : (node->course->prereqs)) {
                    i++;
                    cout << prereq;
                    if (i < node->course->prereqs.size()) {
                        cout << ", ";
                    }
                }
            }
            cout << endl;
        }
        else {
            cout << "Attempt to print invalid node.\n";
        }
    }

    void printSorted() {
        printSorted(root);
        cout << "\nThe courses above are alphanumerically sorted.\n";
    }

    void printSorted(Node* node) {
        if (node != nullptr) {
            printSorted(node->left);
            cout << node->course->courseNumber << " ";
            printSorted(node->right);
        }
    }

    void courseInvalidation() {
        for (auto& course : courses) {
            auto prereq = course->prereqs.begin();
            while (prereq != course->prereqs.end()) {
                // Search for the prereqs in order from beginning to end
                Node* node = search(root, (*prereq));
                if (node == nullptr) {
                    // No node is ever found
                    prereq = course->prereqs.erase(prereq); // TODO: Implement proper erasure
                }
                else {
                    prereq++;
                }
            }
        }
    }

    Node* search(Node* node, const string& courseNumber) { // Snake around in hot pursuit of the target node
        if (node == nullptr || node->course->courseNumber == courseNumber) {
            return node;
        }
        if (courseNumber < node->course->courseNumber) {
            return search(node->left, courseNumber);
        }
        else if (courseNumber > node->course->courseNumber) {
            return search(node->right, courseNumber);
        }
    }

    Node* search(const string& courseNumber) { // Same function, but without a node argument, such that it starts at the root.
        Node* node = root;
        if (node == nullptr || node->course->courseNumber == courseNumber) {
            return node;
        }
        if (courseNumber < node->course->courseNumber) {
            return search(node->left, courseNumber);
        }
    }
};

bool txtParse(BinarySearchTree* BST, const string path) {
    ifstream file(path);
    if (!file) {
        if (!_secureMode) { // If secure mode is disabled, allow the user to try new paths.
            cout << "Invalid path loaded at " << path << ". Try again using a new path?\n";
            string try_again;
            cin >> try_again;
            if (try_again == "y" || try_again == "yes") { // User wants to try again with a new path.
                cout << "New file path: \n";
                string newPath;
                cin >> newPath;
                if (txtParse(BST, newPath)) {
                    return true;
                }
                else
                {
                    try_again = "n";
                }
            }
            else if (try_again == "n" || try_again == "no" || try_again == "q" || try_again == "quit") // User requests not to try again
            {
                return false;
            }
            else { // Recurse down the same path; Essentially, refresh.
                txtParse(BST, path);
            }
        } else {
            cout << "Path not found. Quitting.";
            return false;
        }
    }

    // The file loaded successfully
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string token;
        vector<string> tokens;
        while (getline(iss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() >= 2) {
            Course* newCourse = new Course;
            newCourse->courseNumber = tokens[0];
            newCourse->name = tokens[1];
            for (size_t i = 2; i < tokens.size(); i++) {
                newCourse->prereqs.push_back(tokens[i]);
            }
            
            BST->insert(newCourse);
            cout << newCourse->name << " " << newCourse->courseNumber << " loaded" << endl;
        }
        else {
            cout << "Bad line at " << path << " line " << line << "\n";
        }
    }

    BST->courseInvalidation();
}

void menu() {
    cout << "\n1. Load the target course files.";
    cout << "\n2. Print the loaded courses in alphanumeric order.";
    cout << "\n3. Print a course by Course ID.";
    cout << "\n9. Quit.";
    cout << endl;
    return;
}

int main()
{
    cout << "Welcome!" << endl;
    cout << "Please enter a command.";
    menu();
    BinarySearchTree* BST = new BinarySearchTree; 
    string menuCommand = "";
    int verifiedMenuCommand = 0;
    string queryID;
    while (verifiedMenuCommand != 9) {
        // Validate that the input for the menu is a valid integer to use in the menu switch, in accordance to the menu options.
        cin >> menuCommand;
        transform(menuCommand.begin(), menuCommand.end(), menuCommand.begin(), tolower); // Ensure that the command is not case sensitive by transforming all chars to upper.
        if (menuCommand == "?" || menuCommand == "help" || menuCommand == "menu") { // Show menu.
            menu();
            menuCommand = "";
        }
        else if(menuCommand == "q" || menuCommand == "quit" ) { // Quit.
            verifiedMenuCommand = 9;
        }
        else {
            try
            {
                verifiedMenuCommand = stoi(menuCommand);
                if (verifiedMenuCommand > 9) {
                    throw out_of_range("Out of the menu bounds");
                }
            }
            catch (const out_of_range&)
            {
                menuCommand = "";
            }
            catch (const exception&) {
                menuCommand = "";
            }
        }


        switch (verifiedMenuCommand) {
        case 1:
            txtParse(BST, _filePath);
            cout << "All loaded.\n";
            break;
        case 2:
            BST->printSorted();
            break;
        case 3:
            cout << "Please enter a course ID: \n";
            cin >> queryID;
            transform(queryID.begin(), queryID.end(), queryID.begin(), toupper); // Ensure that the courseId is not case sensitive by transforming all chars to upper.
            if (BST->search(queryID) == nullptr) {
                cout << "Exception: " << queryID << " not found. \n";
            }
            else {
                BST->printCourse(queryID);
                cout << "Course printed.\n";
            }
            break;
        case 9:
            cout << "Thanks for using this program. Goodbye!\n";
            return 0;
        default: 
            cout << "Unrecognized input. ? or help to show menu.\n";
            break;
        }
        verifiedMenuCommand = 0;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
