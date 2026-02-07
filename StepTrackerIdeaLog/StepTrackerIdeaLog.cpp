#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

// ----------- DOCTEST ------------
#ifdef _DEBUG
    #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#else
    #define DOCTEST_CONFIG_DISABLE
#endif
#include "doctest.h"

const int MAX_SESSIONS = 5;

// Enum for character theme
enum CharacterStyle {
    VAMPIRE = 1,
    HUNTER,
    WIZARD
};

// Struct for walk session
struct WalkSession {
    int steps = 0;
    double minutes = 0.0;
    string idea = "";
    CharacterStyle style = VAMPIRE;
};

// ================= FUNCTION PROTOTYPES =================
// IMPORTANT: This must be visible in BOTH debug and non-debug
double calculateStepsPerMinute(const WalkSession& session);

#ifndef _DEBUG
void showBanner();
void showMenu();
int getMenuChoice();
void addSession(WalkSession sessions[], int& count);
void displaySessions(const WalkSession sessions[], int count);
void saveToFile(const WalkSession sessions[], int count);
#endif

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

    double getAverageStepsPerMinute() const {
        if (count == 0) return 0.0;

        double total = 0.0;
        for (int i = 0; i < count; i++)
            total += sessions[i].steps / sessions[i].minutes;

        return total / count;
    }
};

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

    return 0;
}
#endif

// ================= FUNCTIONS =================
#ifndef _DEBUG
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

    int style;
    do {
        cout << "Choose style (1=Vampire, 2=Hunter, 3=Wizard): ";
        cin >> style;
    } while (style < 1 || style > 3);

    sessions[count].style = static_cast<CharacterStyle>(style);
    count++;
}
#endif

// This function is used by BOTH program mode and test mode
double calculateStepsPerMinute(const WalkSession& session) {
    if (session.minutes <= 0)
        return 0.0;
    return session.steps / session.minutes;
}

#ifndef _DEBUG
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

        switch (sessions[i].style) {
        case VAMPIRE:
            cout << "  Vampiric focus achieved.\n";
            break;
        case HUNTER:
            cout << "  Hunter instincts sharpened.\n";
            break;
        case WIZARD:
            cout << "  Wisdom unlocked.\n";
            break;
        }

        if (sessions[i].steps > 3000 && sessions[i].minutes < 30)
            cout << "  High-energy walk detected!\n";
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
#endif

// ================= DOCTEST TESTS =================
#ifdef _DEBUG

TEST_CASE("Steps per minute calculation") {
    WalkSession s{300, 30.0, "", VAMPIRE};
    CHECK(calculateStepsPerMinute(s) == doctest::Approx(10.0));
}

TEST_CASE("Zero minutes guard") {
    WalkSession s{100, 0.0, "", HUNTER};
    CHECK(calculateStepsPerMinute(s) == 0.0);
}

TEST_CASE("Enum assignment works") {
    WalkSession s;
    s.style = WIZARD;
    CHECK(s.style == WIZARD);
}

TEST_CASE("StepTracker adds session") {
    StepTracker tracker;
    CHECK(tracker.addSession({1000, 20, "", HUNTER}));
}

TEST_CASE("Session count increments") {
    StepTracker tracker;
    tracker.addSession({500, 10, "", VAMPIRE});
    CHECK(tracker.getSessionCount() == 1);
}

TEST_CASE("Total steps calculation") {
    StepTracker tracker;
    tracker.addSession({1000, 20, "", VAMPIRE});
    tracker.addSession({2000, 40, "", HUNTER});
    CHECK(tracker.getTotalSteps() == 3000);
}

TEST_CASE("Average steps per minute") {
    StepTracker tracker;
    tracker.addSession({600, 30, "", WIZARD});
    tracker.addSession({1200, 60, "", VAMPIRE});
    CHECK(tracker.getAverageStepsPerMinute() == doctest::Approx(20.0));
}

#endif
