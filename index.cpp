#include <iostream>

using namespace std;

// Define a structure for time
struct Time {
    int hours;
    int minutes;

    Time(int h, int m) : hours(h), minutes(m) {}
};

// Define a basic structure for a landing node
struct LandingNode {
    Time landingTime;
    LandingNode* left;
    LandingNode* right;
    LandingNode* p; // Parent pointer
    int count; // Count of nodes in the subtree rooted at this node

    LandingNode(Time timee) : landingTime(timee), left(nullptr), right(nullptr), p(nullptr), count(1) {}
};

// Reserved landing times
int reservedTimes[1000]; // Assuming a maximum of 1000 reserved times
int reservedCount = 0;

// Function prototypes
void treeInsert(LandingNode*& root, LandingNode* z, int k);
void treeDelete(LandingNode*& root, LandingNode* z);

// In-order tree walk
void inorderTreeWalk(LandingNode* x) {
    if (x != nullptr) {
        inorderTreeWalk(x->left);
        cout << x->landingTime.hours << ":" << x->landingTime.minutes << " ";
        inorderTreeWalk(x->right);
    }
}

// Tree minimum
LandingNode* treeMinimum(LandingNode* x) {
    while (x->left != nullptr)
        x = x->left;
    return x;
}

// Tree maximum
LandingNode* treeMaximum(LandingNode* x) {
    while (x->right != nullptr)
        x = x->right;
    return x;
}


// Corrected Tree successor (Get next landing time)
LandingNode* treeSuccessor(LandingNode* root, LandingNode* x, int userHours, int userMinutes) {
    LandingNode* successor = nullptr;

    while (root != nullptr) {
        if (userHours < root->landingTime.hours ||
            (userHours == root->landingTime.hours && userMinutes < root->landingTime.minutes)) {
            successor = root;
            root = root->left;
        } else {
            root = root->right;
        }
    }

    return successor;
}

// Tree insert with k-minute check
void treeInsert(LandingNode*& root, LandingNode* z, int k) {
    LandingNode* y = nullptr;
    LandingNode* x = root;

    // Check if the landing time already exists with a k-minute check
    while (x != nullptr) {
        y = x;
        x->count++; // Increment the count for nodes being traversed

        // Check if there is a k-minute difference
        int timeDifference = abs(z->landingTime.hours * 60 + z->landingTime.minutes - x->landingTime.hours * 60 - x->landingTime.minutes);
        if (timeDifference <= k) {
            cout << "Error: Landing time conflicts with an existing time slot within " << k << " minutes." << endl;
            delete z; // Clean up memory
            return;
        }

        if (z->landingTime.hours < x->landingTime.hours ||
            (z->landingTime.hours == x->landingTime.hours && z->landingTime.minutes < x->landingTime.minutes)) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->p = y;
    if (y == nullptr)
        root = z; // tree T was empty
    else if (z->landingTime.hours < y->landingTime.hours ||
             (z->landingTime.hours == y->landingTime.hours && z->landingTime.minutes < y->landingTime.minutes))
        y->left = z;
    else
        y->right = z;
}

// Transplant
void transplant(LandingNode*& root, LandingNode* u, LandingNode* v) {
    if (u->p == nullptr)
        root = v;
    else if (u == u->p->left)
        u->p->left = v;
    else
        u->p->right = v;
    if (v != nullptr)
        v->p = u->p;
}

// Tree delete
void treeDelete(LandingNode*& root, LandingNode* z) {
    if (z->left == nullptr)
        transplant(root, z, z->right);
    else if (z->right == nullptr)
        transplant(root, z, z->left);
    else {
        LandingNode* y = treeMinimum(z->right);
        if (y->p != z) {
            transplant(root, y, y->right);
            y->right = z->right;
            y->right->p = y;
        }
        transplant(root, z, y);
        y->left = z->left;
        y->left->p = y;
    }

    // Update counts after deletion
    LandingNode* updateCountNode = z->p;
    while (updateCountNode != nullptr) {
        updateCountNode->count--;
        updateCountNode = updateCountNode->p;
    }

    delete z; // Clean up memory
}

// Rank function
int rank(LandingNode* root, Time timee) {
    int count = 0;
    LandingNode* current = root;

    while (current != nullptr) {
        if (timee.hours < current->landingTime.hours ||
            (timee.hours == current->landingTime.hours && timee.minutes < current->landingTime.minutes)) {
            current = current->left;
        } else {
            count++;
            if (current->left != nullptr)
                count += current->left->count;

            current = current->right;
        }
    }

    return count;
}

// Display the keys of the nodes in the tree
void displayNodes(LandingNode* root) {
    cout << "Landing times: ";
    inorderTreeWalk(root);
    cout << endl;
}

// Reserve function


// Search function
LandingNode* searchLandingTime(LandingNode* root, int userHours, int userMinutes) {
    while (root != nullptr) {
        if (userHours < root->landingTime.hours ||
            (userHours == root->landingTime.hours && userMinutes < root->landingTime.minutes)) {
            root = root->left;
        } else if (userHours > root->landingTime.hours ||
                   (userHours == root->landingTime.hours && userMinutes > root->landingTime.minutes)) {
            root = root->right;
        } else {
            return root; // Found the landing time
        }
    }

    return nullptr; // Landing time not found
}

int main() {
    int option;
    LandingNode* root = nullptr;
    int k; // Declare k outside the loop

    cout << "Enter k minutes: ";
    cin >> k;

do {
        cout << "\nChoose an option:\n";
        cout << "1. Insert a landing time with a k-minute check\n";
        cout << "2. Delete the initial landing time\n";
        cout << "3. Minimum landing time\n";
        cout << "4. Maximum landing time\n";
        cout << "5. Rank of a particular landing time\n";
        cout << "6. Get next landing time\n";
        cout << "7. Display landing times\n";  // Added option for display
        cout << "8. Search landing time\n";
        cout << "9. Quit program\n";

        
        cout << "Enter your option: ";
        cin >> option;

        switch (option) {
           case 1: {
    int hours, minutes;
    cout << "Enter landing time (hours minutes): ";
    cin >> hours >> minutes;

    // Validate hours and minutes
    if (hours < 0 || hours > 24 || minutes < 0 || minutes > 59) {
        cout << "Error: Invalid time. Hours must be between 0 and 24, and minutes must be between 0 and 59." << endl;
        break;
    }

    Time landingTime(hours, minutes);
    //int k = 3; // Assuming k = 3 as per the example
    treeInsert(root, new LandingNode(landingTime), k);
    break;
}

            case 2:
                if (root != nullptr) {
                    cout << "Deleting the initial landing time." << endl;
                    treeDelete(root, treeMinimum(root));
                } else {
                    cout << "No landing time to delete. Tree is empty." << endl;
                }
                break;
            case 3: {
                LandingNode* minNode = treeMinimum(root);
                cout << "Minimum landing time: " << minNode->landingTime.hours << ":" << minNode->landingTime.minutes << endl;
                break;
            }
            case 4: {
                LandingNode* maxNode = treeMaximum(root);
                cout << "Maximum landing time: " << maxNode->landingTime.hours << ":" << maxNode->landingTime.minutes << endl;
                break;
            }
            case 5: {
                int hours, minutes;
                cout << "Enter landing time (hours minutes): ";
                cin >> hours >> minutes;
                Time landingTime(hours, minutes);
                int ranking = ::rank(root, landingTime);

                cout << "Number of planes landing before or at " << landingTime.hours << ":" << landingTime.minutes << ": " << ranking << endl;
                break;
            }
           case 6: {
    int userHours, userMinutes;
    cout << "Enter user-provided landing time (hours minutes): ";
    cin >> userHours >> userMinutes;

    // Validate user-provided hours and minutes
    if (userHours < 0 || userHours > 24 || userMinutes < 0 || userMinutes > 59) {
        cout << "Error: Invalid time. Hours must be between 0 and 24, and minutes must be between 0 and 59." << endl;
        break;
    }

    if (root != nullptr) {
        LandingNode* nextLanding = treeSuccessor(root, root, userHours, userMinutes);
        if (nextLanding != nullptr) {
            cout << "Next landing time: " << nextLanding->landingTime.hours << ":" << nextLanding->landingTime.minutes << endl;
        } else {
            cout << "No next landing time after the provided time." << endl;
        }
    } else {
        cout << "No landing time. Tree is empty." << endl;
    }
    break;
}

            // Add a Display case
case 7: {
    if (root != nullptr && root->count > 0) {
        displayNodes(root);
    } else {
        cout << "No landing times to display. Tree is empty." << endl;
    }
    break;
}

// Case for searching in the main
case 8: {
    int userHours, userMinutes;
    cout << "Enter landing time to search (hours minutes): ";
    cin >> userHours >> userMinutes;

    // Validate user-provided hours and minutes
    if (userHours < 0 || userHours > 24 || userMinutes < 0 || userMinutes > 59) {
        cout << "Error: Invalid time. Hours must be between 0 and 24, and minutes must be between 0 and 59." << endl;
        break;
    }

    LandingNode* foundNode = searchLandingTime(root, userHours, userMinutes);

    if (foundNode != nullptr) {
        cout << "Landing time found: " << foundNode->landingTime.hours << ":" << foundNode->landingTime.minutes << endl;
    } else {
        cout << "Landing time not found." << endl;
    }
    break;
}
            
            case 9:
                // Clean up memory (optional)
                // You would typically have a more sophisticated memory management system
                // Deleting all nodes in the tree
                while (root != nullptr) {
                    treeDelete(root, root);
                }
                return 0; // Exit the program
            default:
                cout << "Invalid option. Please enter a valid option." << endl;
        }}

        // Clear the input buffer
        while (option !=9) ;
            // Continue reading characters until a newline is encountered
        
    
}
