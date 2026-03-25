// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>   // NEW for Week 09

using namespace std;

// ----------- DOCTEST ------------
#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#else
#define DOCTEST_CONFIG_DISABLE
#endif
#include "doctest.h"

const int MAX_SESSIONS = 5; // left for main program compatibility

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

// ================= CLASS =================
class StepTracker {
private:
    vector<WalkSession> sessions;  // ARRAY REPLACED

public:
    StepTracker() {}

    bool addSession(const WalkSession& s) {
        if (s.steps <= 0 || s.minutes <= 0)
            return false;

        sessions.push_back(s);
        return true;
    }

    size_t getSessionCount() const {
        return sessions.size();
    }

    int getTotalSteps() const {
        int total = 0;
        for (size_t i = 0; i < sessions.size(); i++)
            total += sessions.at(i).steps;
        return total;
    }

    // ================= RECURSIVE =================
    int getTotalStepsRecursive(size_t index = 0) const {
        if (index >= sessions.size())
            return 0;

        return sessions.at(index).steps +
            getTotalStepsRecursive(index + 1);
    }

    double getAverageStepsPerMinute() const {
        if (sessions.empty())
            return 0.0;

        double total = 0.0;
        for (size_t i = 0; i < sessions.size(); i++)
            total += sessions.at(i).steps / sessions.at(i).minutes;

        return total / sessions.size();
    }

    // ================= operator[] =================
    WalkSession& operator[](size_t index) {
        if (index >= sessions.size())
            throw StepTrackerException("Invalid index access");

        return sessions.at(index);
    }

    const WalkSession& operator[](size_t index) const {
        if (index >= sessions.size())
            throw StepTrackerException("Invalid index access");

        return sessions.at(index);
    }

    // ================= removeSession =================
    void removeSession(size_t index) {
        if (index >= sessions.size())
            throw StepTrackerException("Invalid removal index");

        sessions.erase(sessions.begin() + index);
    }

    // ================= LINEAR SEARCH =================
    int linearSearchBySteps(int target) const {
        for (size_t i = 0; i < sessions.size(); i++) {
            if (sessions.at(i).steps == target)
                return static_cast<int>(i);
        }
        return -1;
    }

    // ================= BUBBLE SORT =================
    void bubbleSortBySteps() {
        for (size_t i = 0; i < sessions.size(); i++) {
            for (size_t j = 0; j < sessions.size() - 1 - i; j++) {
                if (sessions[j].steps > sessions[j + 1].steps) {
                    swap(sessions[j], sessions[j + 1]);
                }
            }
        }
    }

    // ================= BINARY SEARCH =================
    int binarySearchBySteps(int target) {
        bubbleSortBySteps();  // ensure sorted

        int low = 0;
        int high = static_cast<int>(sessions.size()) - 1;

        while (low <= high) {
            int mid = (low + high) / 2;

            if (sessions.at(mid).steps == target)
                return mid;
            else if (sessions.at(mid).steps < target)
                low = mid + 1;
            else
                high = mid - 1;
        }
        return -1;
    }
};

// ================= FUNCTION PROTOTYPES =================
void showBanner();
void showMenu();
int getMenuChoice();
void addSession(WalkSession sessions[], int& count);
double calculateStepsPerMinute(const WalkSession& session);
void displaySessions(const WalkSession sessions[], int count);
void saveToFile(const WalkSession sessions[], int count);

// ================= NORMAL PROGRAM =================
#ifndef _DEBUG
int main() {
    WalkSession sessions[MAX_SESSIONS];
    int sessionCount = 0;
    int choice;

    showBanner();

    do {
        showMenu();
        choice = getMenuChoice();

        switch (choice) {
        case 1:
            if (sessionCount < MAX_SESSIONS)
                addSession(sessions, sessionCount);
            else
                cout << "Session limit reached.\n";
            break;

        case 2:
            displaySessions(sessions, sessionCount);
            break;

        case 3:
            saveToFile(sessions, sessionCount);
            cout << "Report saved to file.\n";
            break;

        case 4:
            cout << "Goodbye!\n";
            break;

        default:
            cout << "Invalid option.\n";
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
    cout << "3. Save Report\n";
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

void addSession(WalkSession sessions[], int& count) {
    cin.ignore(1000, '\n');

    cout << "Enter idea or reminder: ";
    getline(cin, sessions[count].idea);

    do {
        cout << "Enter steps walked: ";
        cin >> sessions[count].steps;
    } while (sessions[count].steps <= 0);

    do {
        cout << "Enter minutes walked: ";
        cin >> sessions[count].minutes;
    } while (sessions[count].minutes <= 0);

    int styleChoice;
    do {
        cout << "Choose style (1=Vampire, 2=Hunter, 3=Wizard): ";
        cin >> styleChoice;
    } while (styleChoice < 1 || styleChoice > 3);

    sessions[count].style = static_cast<CharacterStyle>(styleChoice);
    count++;
}

double calculateStepsPerMinute(const WalkSession& session) {
    if (session.minutes <= 0)
        return 0.0;

    return session.steps / session.minutes;
}

void displaySessions(const WalkSession sessions[], int count) {
    cout << left << setw(10) << "Steps"
        << setw(10) << "Minutes"
        << setw(15) << "Steps/Min"
        << "Idea\n";

    for (int i = 0; i < count; i++) {
        cout << setw(10) << sessions[i].steps
            << setw(10) << sessions[i].minutes
            << setw(15) << fixed << setprecision(2)
            << calculateStepsPerMinute(sessions[i])
            << sessions[i].idea << "\n";
    }
}

// ================= DOCTEST =================

TEST_CASE("Linear search works") {
    StepTracker t;
    t.addSession({ 1000, 20, "", VAMPIRE });
    t.addSession({ 2000, 30, "", HUNTER });
    CHECK(t.linearSearchBySteps(2000) == 1);
}

TEST_CASE("Binary search works") {
    StepTracker t;
    t.addSession({ 3000, 20, "", VAMPIRE });
    t.addSession({ 1000, 20, "", HUNTER });
    t.addSession({ 2000, 20, "", WIZARD });
    CHECK(t.binarySearchBySteps(2000) != -1);
}

TEST_CASE("Recursive total works") {
    StepTracker t;
    t.addSession({ 1000, 20, "", VAMPIRE });
    t.addSession({ 2000, 20, "", HUNTER });
    CHECK(t.getTotalStepsRecursive() == 3000);
}

TEST_CASE("Empty tracker binary search") {
    StepTracker t;
    CHECK(t.binarySearchBySteps(1000) == -1);
}

TEST_CASE("Bubble sort orders correctly") {
    StepTracker t;
    t.addSession({ 3000, 20, "", VAMPIRE });
    t.addSession({ 1000, 20, "", HUNTER });
    t.addSession({ 2000, 20, "", WIZARD });

    t.bubbleSortBySteps();

    CHECK(t[0].steps == 1000);
    CHECK(t[1].steps == 2000);
    CHECK(t[2].steps == 3000);
}

TEST_CASE("Bubble sort orders correctly") {
    StepTracker t;
    t.addSession({ 3000, 20, "", VAMPIRE });
    t.addSession({ 1000, 20, "", HUNTER });
    t.addSession({ 2000, 20, "", WIZARD });

    t.bubbleSortBySteps();

    CHECK(t[0].steps == 1000);
    CHECK(t[1].steps == 2000);
    CHECK(t[2].steps == 3000);
}

TEST_CASE("Linear search returns -1 when not found") {
    StepTracker t;
    t.addSession({ 1000, 20, "", VAMPIRE });
    CHECK(t.linearSearchBySteps(9999) == -1);
}