// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include<map>

#include "HttpClient.h"
#include "json.hpp"

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
//derived rest client:

class MacombAPIclient : public HttpClient {
private:
    string response_;
protected:
    void StartOfData() override {
        response_.clear();
    }
    void Data(const char* data, const unsigned int size) override {
        response_.append(data, data + size);

    }
    void EndOfData() override {

    }
public:
    const string& GetResponse() const {
        return response_;
    }
};

//Json:
static vector<WalkSession> ParseSeedFileToSessions(const string& fileText) {
    vector<WalkSession> sessions;
    try {
        const nlohmann::json root = nlohmann::json::parse(fileText);
        if (!root.is_array()) {
            return {};
        }
        for (const auto& item : root) {
            WalkSession s;
            s.steps = item.value("steps", 0);
            s.minutes = item.value("minutes", 0.0);
            s.idea = item.value("idea", "");
            s.style = static_cast<CharacterStyle>(item.value("style", static_cast<int>(VAMPIRE)));

            sessions.push_back(s);
        }
    }
    catch (const nlohmann::json::exception&) {
        return {};
    }
    return sessions;
}
static bool TryGetTriviaArray(const nlohmann::json & root, nlohmann::json & outArray) {
    if (root.is_array()) {
        outArray = root;
		return true;
    }
    if (root.is_object()) {
        if (root.contains("items") && root["items"].is_array()) {
            outArray = root["items"];
            return true;
        }
        if (root.contains("data") && root["data"].is_array()) {
            outArray = root["data"];
            return true;
        }
        if (root.contains("trivia") && root["trivia"].is_array()) {
            outArray = root["trivia"];
            return true;
        }
    }

    return false;
};
static vector<WalkSession> ParseTriviaToSessions(const string& responsibility, int startingSteps) {
    vector<WalkSession> sessions;
    try {
        const nlohmann::json root = nlohmann::json::parse(responsibility);
        nlohmann::json triviaArray;
        if (!TryGetTriviaArray(root, triviaArray)) {
            return {};
        }
        int steps = startingSteps;
        for (const auto& item : triviaArray) {
            const string question = item.value("question", "");
            const string answer = item.value("answer", "");
            const string category = item.value("category", "");

            std::ostringstream ideaText;
            if (!category.empty()) {
                ideaText << "[" << category << "] ";
            }
            ideaText << question;
            if (!answer.empty()) {
                ideaText << " (Answer: " << answer << ")";
            }
            WalkSession s;
            s.steps = steps++;
            s.minutes = 1.0;
            s.idea = ideaText.str();
            s.style = WIZARD;

            sessions.push_back(s);
        }
        
    }
    catch (const nlohmann::json::exception&) {
        return {};
    }
	return sessions;
};

static string TryExtractAssignedId(const string& responseBody) {
    try {
		const nlohmann::json root = nlohmann::json::parse(responseBody);
        if (!root.is_object()) {
            return "";
		}
        const char* keysToCheck[] = { "id", "assignedId", "Id", "ID" };
        for (const auto* key : keysToCheck) {
            if (!root.contains(key)) {
                continue;
            }
            const auto& v = root[key];
            if (v.is_number_integer()) return to_string(v.get<int>());
            if (v.is_string()) return v.get<string>();
        }
    }
    catch (const nlohmann::json::exception&) {
        return "";
	}
	return "";
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
	int nextApiSteps_ = 1000; 
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
    //Json:
    int importSessionsFromJsonFile(const string& filePath) {
        try {
            ifstream file(filePath);
            if (!file) {
                throw StepTrackerException("JSON file not found: " + filePath);
            }

            ostringstream buffer;
            buffer << file.rdbuf();

            const vector<WalkSession> loaded = ParseSeedFileToSessions(buffer.str());

            int added = 0;
            for (const auto& s : loaded) {
                if (addSession(s)) {
                    added++;
                }
            }

            return added;
        }
        catch (const StepTrackerException&) {
            return 0;
        }
        catch (const nlohmann::json::exception&) {
            return 0;
        }
    }
   //restAPI:
    int importTriviaSessionsFromApiResponse(const string& responseBody) {
        const vector<WalkSession> imported = ParseTriviaToSessions(responseBody, nextApiSteps_);

        int added = 0;
        for (const auto& s : imported) {
            if (addSession(s)) {
                added++;
                nextApiSteps_++;
            }
        }

        return added;
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
            const int added = tracker.importSessionsFromJsonFile("WalkSessionsSeed.json");
            cout << "Loaded " << added << " sessions from WalkSessionsSeed.json"<<endl;
        }
        else if (choice == 5) {
            MacombAPIclient api;

            if (!api.Connect("api.macomb.io", INTERNET_DEFAULT_HTTP_PORT)) {
                cout << "Failed to connect to api.macomb.io (HTTP)."<<endl;
                continue;
            }

            const map<string, string> qp = {
                {"count", "3"},
                {"difficulty", "easy"}
            };

            if (!api.Get("/trivia", qp)) {
                cout << "GET /trivia failed."<<endl;
                continue;
            }

            const int added = tracker.importTriviaSessionsFromApiResponse(api.GetResponse());
            cout << "Imported " << added << " trivia items into your sessions."<<endl;
        }
        else if (choice == 6) {
            cin.ignore(1000, '\n');

            string question;
            string answer;

            cout << "Enter trivia question: ";
            getline(cin, question);

            cout << "Enter trivia answer: ";
            getline(cin, answer);

            nlohmann::json requestBody = {
                {"question", question},
                {"answer", answer},
                {"category", "general"},
                {"difficulty", "easy"}
            };

            MacombAPIclient api;

            if (!api.Connect("api.macomb.io", INTERNET_DEFAULT_HTTP_PORT)) {
                cout << "Failed to connect to api.macomb.io (HTTP)."<<endl;
                continue;
            }

            if (!api.Post("/trivia", requestBody.dump())) {
                cout << "POST /trivia failed."<<endl;
                continue;
            }

            const string assignedId = TryExtractAssignedId(api.GetResponse());
            if (assignedId.empty()) {
                cout << "POST succeeded, but could not parse assigned ID from response."<<endl;
            }
            else {
                cout << "Server assigned ID: " << assignedId << endl;
            }
        }
        else if (choice == 7) {
			cout << "Goodbye!" << endl;
        }

    } while (choice != 7);

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
	cout << "4. Load Sessions from JSON File\n";
    cout << "5. Import Trivia Ideas (GET from API)\n";
    cout << "6. Add Trivia Item (POST to API)\n";
    cout << "7. Exit\n";
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
TEST_CASE("ParseSeedFileToSessions parses array") {
    const string jsonText = R"([
        { "steps": 100, "minutes": 10.0, "idea": "A", "style": 1 },
        { "steps": 200, "minutes": 20.0, "idea": "B", "style": 2 },
        { "steps": 300, "minutes": 30.0, "idea": "C", "style": 3 },
        { "steps": 400, "minutes": 40.0, "idea": "D", "style": 1 },
        { "steps": 500, "minutes": 50.0, "idea": "E", "style": 2 }
    ])";

    const auto sessions = ParseSeedFileToSessions(jsonText);
    CHECK(sessions.size() == 5);
    CHECK(sessions[0].steps == 100);
    CHECK(sessions[4].minutes == doctest::Approx(50.0));
}

TEST_CASE("importSessionsFromJsonFile returns 0 for missing file") {
    StepTracker tracker;
    CHECK(tracker.importSessionsFromJsonFile("this_file_should_not_exist_12345.json") == 0);
}

TEST_CASE("ParseTriviaToSessions parses trivia array and creates WalkSession objects") {
    const string jsonText = R"([
        { "question": "What is 2+2?", "answer": "4", "category": "math", "difficulty": "easy" },
        { "question": "Capital of Michigan?", "answer": "Lansing", "category": "geography", "difficulty": "easy" }
    ])";

    const auto sessions = ParseTriviaToSessions(jsonText, 1000000);

    CHECK(sessions.size() == 2);
    CHECK(sessions[0].steps == 1000000);
    CHECK(sessions[1].steps == 1000001);
    CHECK(sessions[0].minutes == doctest::Approx(1.0));
    CHECK(sessions[0].idea.find("What is 2+2?") != string::npos);
    CHECK(sessions[0].idea.find("Answer: 4") != string::npos);
}

TEST_CASE("ParseTriviaToSessions handles invalid JSON and returns empty") {
    const string badJson = R"({ this is not valid JSON )";
    const auto sessions = ParseTriviaToSessions(badJson, 1000000);
    CHECK(sessions.empty());
}

TEST_CASE("TryExtractAssignedId reads common id fields from a POST response") {
    CHECK(TryExtractAssignedId(R"({ "id": 123 })") == "123");
    CHECK(TryExtractAssignedId(R"({ "assignedId": "abc-123" })") == "abc-123");
    CHECK(TryExtractAssignedId(R"(not json)") == "");
}