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

public:
    bool addSession(const WalkSession& s) {
        if (s.steps <= 0 || s.minutes <= 0)
            return false;
        sessions.insertBack(s);
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