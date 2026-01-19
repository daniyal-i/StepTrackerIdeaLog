#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

const int MAX_SESSIONS = 5;

// Enum for character theme
enum CharacterStyle {
    VAMPIRE = 1,
    HUNTER,
    WIZARD
};

// Struct for walk session (initialized to avoid warnings)
struct WalkSession {
    int steps = 0;
    double minutes = 0.0;
    string idea = "";
    CharacterStyle style = VAMPIRE;
};

// Function prototypes
void showBanner();
void showMenu();
int getMenuChoice();
void addSession(WalkSession sessions[], int& count);
double calculateStepsPerMinute(const WalkSession& session);
void displaySessions(const WalkSession sessions[], int count);
void saveToFile(const WalkSession sessions[], int count);

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
            if (sessionCount < MAX_SESSIONS) {
                addSession(sessions, sessionCount);
            }
            else {
                cout << "Session limit reached.\n";
            }
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

// Displays banner
void showBanner() {
    cout << "=====================================\n";
    cout << "   Step Tracker and Idea Log Program\n";
    cout << "=====================================\n";
}

// Displays menu
void showMenu() {
    cout << "\n1. Add Walking Session\n";
    cout << "2. View Sessions\n";
    cout << "3. Save Report\n";
    cout << "4. Exit\n";
}

// Menu input validation
int getMenuChoice() {
    int choice;
    while (!(cin >> choice)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Enter a valid number: ";
    }
    return choice;
}

// Adds a walking session
void addSession(WalkSession sessions[], int& count) {
    cin.ignore();

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

// Calculation function
double calculateStepsPerMinute(const WalkSession& session) {
    return session.steps / session.minutes;
}

// Displays all sessions
void displaySessions(const WalkSession sessions[], int count) {
    cout << left << setw(10) << "Steps"
        << setw(10) << "Minutes"
        << setw(15) << "Steps/Min"
        << "Idea\n";

    cout << "--------------------------------------------\n";

    for (int i = 0; i < count; i++) {
        cout << setw(10) << sessions[i].steps
            << setw(10) << sessions[i].minutes
            << setw(15) << fixed << setprecision(2)
            << calculateStepsPerMinute(sessions[i])
            << sessions[i].idea << "\n";

        // Enum used in decision structure
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

        // Compound condition
        if (sessions[i].steps > 3000 && sessions[i].minutes < 30) {
            cout << "  High-energy walk detected!\n";
        }
    }
}

// Saves report to file
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