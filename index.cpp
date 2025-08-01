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
    int count;      // Count of nodes in the subtree rooted at this node

    LandingNode(Time time) : landingTime(time), left(nullptr), right(nullptr), p(nullptr), count(1) {}
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

// Tree successor (Get next landing time)
LandingNode* treeSuccessor(LandingNode* x) {
    if (x->right != nullptr)
        return treeMinimum(x->right);
    LandingNode* y = x->p;
    while (y != nullptr && x == y->right) {
        x = y;
        y = y->p;
    }
    return y;
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
int rank(LandingNode* root, Time time) {
    int count = 0;
    LandingNode* current = root;

    while (current != nullptr) {
        if (time.hours < current->landingTime.hours ||
            (time.hours == current->landingTime.hours && time.minutes < current->landingTime.minutes)) {
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
bool reserveLandingTime(int requestedTime, int k) {
    if (requestedTime < 0) {
        cout << "Error: Requested time is in the past." << endl;
        return false;
    }

    for (int i = 0; i < reservedCount; ++i) {
        if (abs(requestedTime - reservedTimes[i]) < k) {
            cout << "Error: Requested time conflicts with an existing reservation." << endl;
            return false;
        }
    }

    reservedTimes[reservedCount++] = requestedTime;
    // Sort reservedTimes (you can implement a simple sorting algorithm or use an existing one)
    for (int i = 0; i < reservedCount - 1; ++i) {
        for (int j = i + 1; j < reservedCount; ++j) {
            if (reservedTimes[i] > reservedTimes[j]) {
                int temp = reservedTimes[i];
                reservedTimes[i] = reservedTimes[j];
                reservedTimes[j] = temp;
            }
        }
    }

    return true;
}

// Land function
bool landPlane(LandingNode*& root, int now) {
    if (reservedCount == 0) {
        cout << "Error: No reserved landing times. Cannot land a plane." << endl;
        return false;
    }

    int earliestTime = reservedTimes[0];
    if (earliestTime != now) {
        cout << "Error: No plane scheduled to land at the current time." << endl;
        return false;
    }

    // Remove the earliest landing time from reservedTimes
    for (int i = 1; i < reservedCount; ++i) {
        reservedTimes[i - 1] = reservedTimes[i];
    }
    reservedCount--;

    // Find the node with the earliest landing time and delete it from the tree
    LandingNode* earliestNode = treeMinimum(root);
    treeDelete(root, earliestNode);

    cout << "Plane landed successfully at time: " << now << endl;

    return true;
}

int main() {
    int option;
    LandingNode* root = nullptr;

do {
        cout << "\nChoose an option:\n";
        cout << "1. Insert a landing time with a k-minute check\n";
        cout << "2. Delete the initial landing time\n";
        cout << "3. Minimum landing time\n";
        cout << "4. Maximum landing time\n";
        cout << "5. Rank of a particular landing time\n";
        cout << "6. Get next landing time\n";
        cout << "7. Reserve a landing time\n";
        cout << "8. Land the plane\n";
        cout << "9. Quit program\n";

        
        cout << "Enter your option: ";
        cin >> option;

        switch (option) {
            case 1: {
                int hours, minutes;
                cout << "Enter landing time (hours minutes): ";
                cin >> hours >> minutes;
                Time landingTime(hours, minutes);
                int k = 3; // Assuming k = 3 as per the example
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
                if (root != nullptr) {
                    LandingNode* nextLanding = treeSuccessor(treeMinimum(root));
                    cout << "Next landing time: " << nextLanding->landingTime.hours << ":" << nextLanding->landingTime.minutes << endl;
                } else {
                    cout << "No landing time. Tree is empty." << endl;
                }
                break;
            }
            case 7: {
                int requestedTime;
                cout << "Enter requested landing time: ";
                cin >> requestedTime;
                int k = 3; // Assuming k = 3 as per the example
                reserveLandingTime(requestedTime, k);
                break;
            }
            case 8: {
                int now;
                cout << "Enter current time: ";
                cin >> now;
                landPlane(root, now);
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

