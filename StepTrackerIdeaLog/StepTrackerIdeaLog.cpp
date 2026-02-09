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

// ================= ENUM =================
enum CharacterStyle {
    VAMPIRE = 1,
    HUNTER,
    WIZARD
};

// ================= STRUCT =================
struct WalkSession {
    int steps = 0;
    double minutes = 0.0;
    string idea = "";
    CharacterStyle style = VAMPIRE;
};

// ================= FUNCTION PROTOTYPES =================
double calculateStepsPerMinute(const WalkSession& session);

#ifndef _DEBUG
void showBanner();
void showMenu();
int getMenuChoice();
void addSession(WalkSession sessions[], int& count);
void displaySessions(const WalkSession sessions[], int count);
void saveToFile(const WalkSession sessions[], int count);
#endif

// =================================================================
// ======================= WEEK 04 OOP ADDITIONS ====================
// =================================================================

// ================= BASE CLASS =================
class Activity {
protected:
    string name;
    int duration;
    CharacterStyle style;

public:
    Activity() : name(""), duration(0), style(VAMPIRE) {}

    Activity(string n, int d, CharacterStyle s)
        : name(n), duration(d), style(s) {}

    string getName() const { return name; }
    int getDuration() const { return duration; }
    CharacterStyle getStyle() const { return style; }

    virtual void print() const {
        cout << "Activity: " << name
             << " | Duration: " << duration << " minutes\n";
    }
};

// ================= COMPOSITION CLASS =================
class Metrics {
private:
    int steps;

public:
    Metrics() : steps(0) {}
    Metrics(int s) : steps(s) {}

    int getSteps() const { return steps; }
    void setSteps(int s) { steps = s; }

    double stepsPerMinute(int minutes) const {
        if (minutes <= 0) return 0.0;
        return static_cast<double>(steps) / minutes;
    }
};

// ================= DERIVED CLASS 1 =================
class WalkActivity : public Activity {
private:
    Metrics metrics;

public:
    WalkActivity() : Activity(), metrics() {}

    WalkActivity(string n, int d, CharacterStyle s, int steps)
        : Activity(n, d, s), metrics(steps) {}

    void print() const override {
        Activity::print();
        cout << "Type: Walk\n";
        cout << "Steps: " << metrics.getSteps()
             << " | Steps/min: "
             << metrics.stepsPerMinute(duration) << "\n";
    }
};

// ================= DERIVED CLASS 2 =================
class RunActivity : public Activity {
private:
    Metrics metrics;
    bool treadmill;

public:
    RunActivity() : Activity(), metrics(), treadmill(false) {}

    RunActivity(string n, int d, CharacterStyle s, int steps, bool t)
        : Activity(n, d, s), metrics(steps), treadmill(t) {}

    void print() const override {
        Activity::print();
        cout << "Type: Run\n";
        cout << "Steps: " << metrics.getSteps()
             << " | Steps/min: "
             << metrics.stepsPerMinute(duration) << "\n";
        cout << "Treadmill: " << (treadmill ? "Yes" : "No") << "\n";
    }
};

// ================= EXISTING CLASS =================
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
    }
}

void saveToFile(const WalkSession sessions[], int count) {
    ofstream file("walk_report.txt");

    for (int i = 0; i < count; i++) {
        file << sessions[i].steps << " "
             << sessions[i].minutes << " "
             << calculateStepsPerMinute(sessions[i]) << " "
             << sessions[i].idea << "\n";
    }

    file.close();
}
#endif

// ================= DOCTEST TESTS =================
#ifdef _DEBUG

TEST_CASE("Base Activity constructor") {
    Activity a("Test", 30, VAMPIRE);
    CHECK(a.getDuration() == 30);
}

TEST_CASE("Metrics helper function") {
    Metrics m(300);
    CHECK(m.stepsPerMinute(30) == doctest::Approx(10.0));
}

TEST_CASE("WalkActivity inheritance") {
    WalkActivity w("Walk", 20, HUNTER, 2000);
    CHECK(w.getDuration() == 20);
}

TEST_CASE("RunActivity adds new data") {
    RunActivity r("Run", 15, WIZARD, 1800, true);
    CHECK(r.getDuration() == 15);
}

TEST_CASE("StepTracker total steps") {
    StepTracker tracker;
    tracker.addSession({1000, 20, "", VAMPIRE});
    tracker.addSession({2000, 40, "", HUNTER});
    CHECK(tracker.getTotalSteps() == 3000);
}

TEST_CASE("calculateStepsPerMinute guard") {
    WalkSession s{100, 0.0, "", HUNTER};
    CHECK(calculateStepsPerMinute(s) == 0.0);
}

#endif
