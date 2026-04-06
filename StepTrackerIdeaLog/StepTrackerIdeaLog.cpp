// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;

// ----------- DOCTEST ------------
#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#else
#define DOCTEST_CONFIG_DISABLE
#endif
#include "doctest.h"

// ================= ENUM =================
enum CharacterStyle {
    VAMPIRE = 1,
    HUNTER,
    WIZARD
};

// ================= CUSTOM EXCEPTION =================
class StepTrackerException : public std::runtime_error {
public:
    StepTrackerException(const std::string& message)
        : std::runtime_error(message) {}
};

// ================= STRUCT =================
struct WalkSession {
    int steps = 0;
    double minutes = 0.0;
    string idea = "";
    CharacterStyle style = VAMPIRE;
};

// DN Added array based stack class 
class SessionStack {
private:
    static const int MAX_SIZE = 100;
    WalkSession items[MAX_SIZE];
    int topIndex;

public:
    SessionStack() : topIndex(-1) {}

    bool isEmpty() const {
        return topIndex == -1;
    }

    bool isFull() const {
        return topIndex == MAX_SIZE - 1;
    }

    void push(const WalkSession& s) {
        if (isFull())
            throw StepTrackerException("Stack is full.");
        topIndex++;
        items[topIndex] = s;
    }

    void pop() {
        if (isEmpty())
            throw StepTrackerException("Stack is empty.");
        topIndex--;
    }

    WalkSession top() const {
        if (isEmpty())
            throw StepTrackerException("Stack is empty.");
        return items[topIndex];
    }
};

// DN 2: Added a custom linked list queue class existing LinkedList class
class SessionQueue {
private:
    struct QueueNode {
        WalkSession data;
        QueueNode* next;
        QueueNode(const WalkSession& s) : data(s), next(nullptr) {}
    };

    QueueNode* frontPtr;
    QueueNode* backPtr;

public:
    SessionQueue() : frontPtr(nullptr), backPtr(nullptr) {}

    ~SessionQueue() {
        while (!isEmpty())
            dequeue();
    }

    bool isEmpty() const {
        return frontPtr == nullptr;
    }

    void enqueue(const WalkSession& s) {
        QueueNode* newNode = new QueueNode(s);

        if (isEmpty()) {
            frontPtr = newNode;
            backPtr = newNode;
        }
        else {
            backPtr->next = newNode;
            backPtr = newNode;
        }
    }

    void dequeue() {
        if (isEmpty())
            throw StepTrackerException("Queue is empty.");

        QueueNode* temp = frontPtr;
        frontPtr = frontPtr->next;
        delete temp;

        if (frontPtr == nullptr)
            backPtr = nullptr;
    }

    WalkSession front() const {
        if (isEmpty())
            throw StepTrackerException("Queue is empty.");
        return frontPtr->data;
    }
};

// =====================================================
// ================= LINKED LIST ADT ===================
// =====================================================
class LinkedList {
private:
    struct Node {
        WalkSession data;
        Node* next;
        Node(const WalkSession& s) : data(s), next(nullptr) {}
    };

    Node* head;

public:
    // Constructor
    LinkedList() : head(nullptr) {}

    // Destructor
    ~LinkedList() {
        clear();
    }

    // Clear entire list
    void clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
    }

    // Insert at front
    void insertFront(const WalkSession& s) {
        Node* newNode = new Node(s);
        newNode->next = head;
        head = newNode;
    }

    // Insert at back
    void insertBack(const WalkSession& s) {
        Node* newNode = new Node(s);

        if (head == nullptr) {
            head = newNode;
            return;
        }

        Node* current = head;
        while (current->next != nullptr)
            current = current->next;

        current->next = newNode;
    }

    // Remove node by steps value
    bool remove(int steps) {
        if (head == nullptr) return false;

        if (head->data.steps == steps) {
            Node* temp = head;
            head = head->next;
            delete temp;
            return true;
        }

        Node* current = head;
        while (current->next != nullptr && current->next->data.steps != steps)
            current = current->next;

        if (current->next == nullptr) return false;

        Node* temp = current->next;
        current->next = temp->next;
        delete temp;
        return true;
    }

    // Search node by steps value
    int search(int steps) const {
        Node* current = head;
        int index = 0;

        while (current != nullptr) {
            if (current->data.steps == steps)
                return index;
            current = current->next;
            index++;
        }
        return -1;
    }

    // Traverse and print
    void print() const {
        Node* current = head;
        while (current != nullptr) {
            cout << current->data.steps << " steps, "
                << current->data.minutes << " minutes, "
                << current->data.idea << endl;
            current = current->next;
        }
    }

    // Get total steps (recursive)
    int getTotalStepsRecursive(Node* node) const {
        if (node == nullptr)
            return 0;
        return node->data.steps + getTotalStepsRecursive(node->next);
    }

    int getTotalStepsRecursive() const {
        return getTotalStepsRecursive(head);
    }

    // ================= ITERATOR =================
    class Iterator {
    private:
        Node* current;
    public:
        Iterator(Node* start) : current(start) {}

        bool hasNext() const {
            return current != nullptr;
        }

        WalkSession& getData() {
            return current->data;
        }

        void next() {
            if (current != nullptr)
                current = current->next;
        }
    };

    Iterator begin() {
        return Iterator(head);
    }
};

// =====================================================
// ================= STEP TRACKER CLASS =================
// =====================================================
class StepTracker {
private:
    LinkedList sessions;  // Replaced vector with linked list
    SessionStack recentSessions;

public:
    bool addSession(const WalkSession& s) {
        if (s.steps <= 0 || s.minutes <= 0)
            return false;
        sessions.insertBack(s);
        // DN 3: Store each successful session on the custom stack so the
        // existing program now uses ADTs 
        recentSessions.push(s);
        return true;
    }

    bool removeSession(int steps) {
        return sessions.remove(steps);
    }

    int searchSession(int steps) const {
        return sessions.search(steps);
    }

    int getTotalStepsRecursive() const {
        return sessions.getTotalStepsRecursive();
    }

    // DN 4: Added a small accessor so doctests can verify the stack is being
    // used by StepTracker without rewriting
    WalkSession getMostRecentSession() const {
        return recentSessions.top();
    }

    void displaySessions() const {
        sessions.print();
    }
};

// =====================================================
// ================= FUNCTION PROTOTYPES ===============
// =====================================================
void showBanner();
void showMenu();
int getMenuChoice();

// ================= NORMAL PROGRAM =================
#ifndef _DEBUG
int main() {
    StepTracker tracker;
    int choice;

    showBanner();

    do {
        showMenu();
        choice = getMenuChoice();

        if (choice == 1) {
            WalkSession s;
            cin.ignore(1000, '\n');

            cout << "Enter idea or reminder: ";
            getline(cin, s.idea);

            cout << "Enter steps walked: ";
            cin >> s.steps;

            cout << "Enter minutes walked: ";
            cin >> s.minutes;

            int styleChoice;
            cout << "Choose style (1=Vampire, 2=Hunter, 3=Wizard): ";
            cin >> styleChoice;
            s.style = static_cast<CharacterStyle>(styleChoice);

            tracker.addSession(s);
        }
        else if (choice == 2) {
            tracker.displaySessions();
        }
        else if (choice == 3) {
            cout << "Enter steps to remove: ";
            int steps;
            cin >> steps;
            tracker.removeSession(steps);
        }
        else if (choice == 4) {
            cout << "Goodbye!\n";
        }

    } while (choice != 4);

    // _CrtDumpMemoryLeaks();
    return 0;
}
#endif

// ================= FUNCTIONS =================
void showBanner() {
    cout << "=====================================\n";
    cout << "   Step Tracker and Idea Log Program\n";
    cout << "=====================================\n";
}

void showMenu() {
    cout << "\n1. Add Walking Session\n";
    cout << "2. View Sessions\n";
    cout << "3. Remove Session\n";
    cout << "4. Exit\n";
}

int getMenuChoice() {
    int choice;
    while (!(cin >> choice)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Enter a valid number: ";
    }
    return choice;
}

// =====================================================
// ======================= DOCTESTS =====================
// =====================================================

TEST_CASE("Insert into empty list") {
    StepTracker tracker;
    CHECK(tracker.addSession({ 1000, 20, "Morning Walk", VAMPIRE }) == true);
}

TEST_CASE("Search existing session") {
    StepTracker tracker;
    tracker.addSession({ 2000, 30, "Evening Walk", HUNTER });
    CHECK(tracker.searchSession(2000) == 0);
}

TEST_CASE("Search non-existing session") {
    StepTracker tracker;
    tracker.addSession({ 1500, 25, "Lunch Walk", WIZARD });
    CHECK(tracker.searchSession(9999) == -1);
}

TEST_CASE("Delete existing session") {
    StepTracker tracker;
    tracker.addSession({ 3000, 40, "Night Walk", VAMPIRE });
    CHECK(tracker.removeSession(3000) == true);
}

TEST_CASE("Delete non-existing session") {
    StepTracker tracker;
    tracker.addSession({ 1200, 15, "Quick Walk", HUNTER });
    CHECK(tracker.removeSession(9999) == false);
}

TEST_CASE("Recursive total steps calculation") {
    StepTracker tracker;
    tracker.addSession({ 1000, 20, "", VAMPIRE });
    tracker.addSession({ 2000, 30, "", HUNTER });
    CHECK(tracker.getTotalStepsRecursive() == 3000);
}

TEST_CASE("Traverse empty list") {
    StepTracker tracker;
    CHECK(tracker.getTotalStepsRecursive() == 0);
}

// DN 5-A: Added custom stack operations
TEST_CASE("Stack starts empty") {
    SessionStack stack;
    CHECK(stack.isEmpty() == true);
}

TEST_CASE("Stack push and top") {
    SessionStack stack;
    stack.push({ 1111, 10, "Top item", VAMPIRE });
    CHECK(stack.top().steps == 1111);
}

TEST_CASE("Stack pop removes top item") {
    SessionStack stack;
    stack.push({ 1111, 10, "First", VAMPIRE });
    stack.push({ 2222, 20, "Second", HUNTER });
    stack.pop();
    CHECK(stack.top().steps == 1111);
}

TEST_CASE("Stack pop on empty throws exception") {
    SessionStack stack;
    CHECK_THROWS_AS(stack.pop(), StepTrackerException);
}

TEST_CASE("Stack push on full throws exception") {
    SessionStack stack;
    for (int i = 0; i < 100; i++) {
        stack.push({ i + 1, 10, "Fill stack", VAMPIRE });
    }
    CHECK_THROWS_AS(stack.push({ 101, 10, "Overflow", HUNTER }), StepTrackerException);
}

// DN 5-B: Added doctests for the custom queue operations 
TEST_CASE("Queue starts empty") {
    SessionQueue queue;
    CHECK(queue.isEmpty() == true);
}

TEST_CASE("Queue enqueue and front") {
    SessionQueue queue;
    queue.enqueue({ 3333, 30, "Front item", WIZARD });
    CHECK(queue.front().steps == 3333);
}

TEST_CASE("Queue dequeue advances front") {
    SessionQueue queue;
    queue.enqueue({ 3333, 30, "First", WIZARD });
    queue.enqueue({ 4444, 40, "Second", HUNTER });
    queue.dequeue();
    CHECK(queue.front().steps == 4444);
}

TEST_CASE("Queue dequeue on empty throws exception") {
    SessionQueue queue;
    CHECK_THROWS_AS(queue.dequeue(), StepTrackerException);
}

// DN 5-C: Added one integration test to prove StepTracker now uses the custom stack.
TEST_CASE("StepTracker stores the most recent session on the stack") {
    StepTracker tracker;
    tracker.addSession({ 5000, 50, "First walk", VAMPIRE });
    tracker.addSession({ 6000, 60, "Most recent walk", WIZARD });
    CHECK(tracker.getMostRecentSession().steps == 6000);
}
