// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>   // NEW for exceptions

using namespace std;

// ----------- DOCTEST ------------
#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#else
#define DOCTEST_CONFIG_DISABLE
#endif
#include "doctest.h"

const int MAX_SESSIONS = 5;

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
    WalkSession sessions[MAX_SESSIONS];
    int count;

public:
    StepTracker() : count(0) {}

    bool addSession(const WalkSession& s) {
        if (count >= MAX_SESSIONS || s.steps <= 0 || s.minutes <= 0)
            return false;

        sessions[count++] = s;
        return true;
    }

    int getSessionCount() const {
        return count;
    }

    int getTotalSteps() const {
        int total = 0;
        for (int i = 0; i < count; i++)
            total += sessions[i].steps;
        return total;
    }

    // ================= NEW RECURSIVE FUNCTION =================
    int getTotalStepsRecursive(int index = 0) const {
        // Base case
        if (index >= count)
            return 0;

        // Recursive case
        return sessions[index].steps +
            getTotalStepsRecursive(index + 1);
    }

    double getAverageStepsPerMinute() const {
        if (count == 0)
            return 0.0;

        double total = 0.0;
        for (int i = 0; i < count; i++)
            total += sessions[i].steps / sessions[i].minutes;

        return total / count;
    }

    // ================= operator[] =================
    WalkSession& operator[](int index) {
        if (index < 0 || index >= count)
            throw StepTrackerException("Invalid index access");

        return sessions[index];
    }

    const WalkSession& operator[](int index) const {
        if (index < 0 || index >= count)
            throw StepTrackerException("Invalid index access");

        return sessions[index];
    }

    // ================= removeSession =================
    void removeSession(int index) {
        if (index < 0 || index >= count)
            throw StepTrackerException("Invalid removal index");

        for (int i = index; i < count - 1; i++)
            sessions[i] = sessions[i + 1];

        count--;
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

void saveToFile(const WalkSession sessions[], int count) {
    ofstream file("walk_report.txt");

    file << left << setw(10) << "Steps"
        << setw(10) << "Minutes"
        << setw(15) << "Steps/Min"
        << "Idea\n";

    for (int i = 0; i < count; i++) {
        file << setw(10) << sessions[i].steps
            << setw(10) << sessions[i].minutes
            << setw(15) << fixed << setprecision(2)
            << calculateStepsPerMinute(sessions[i])
            << sessions[i].idea << "\n";
    }

    file.close();
}

// ================= DOCTEST TESTS =================

TEST_CASE("Steps per minute calculation") {
    WalkSession s{ 300, 30.0, "", VAMPIRE };
    CHECK(calculateStepsPerMinute(s) == doctest::Approx(10.0));
}

TEST_CASE("Invalid index throws exception") {
    StepTracker tracker;
    CHECK_THROWS(tracker[0]);
}

TEST_CASE("Invalid removal throws exception") {
    StepTracker tracker;
    CHECK_THROWS(tracker.removeSession(0));
}

TEST_CASE("Recursive total steps works") {
    StepTracker tracker;
    tracker.addSession({ 1000, 20, "", VAMPIRE });
    tracker.addSession({ 2000, 30, "", HUNTER });
    CHECK(tracker.getTotalStepsRecursive() == 3000);
}