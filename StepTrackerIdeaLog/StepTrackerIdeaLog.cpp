#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

// ----------- DOCTEST ------------
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

const int MAX_SESSIONS = 5;

// ================= ENUM =================
enum CharacterStyle {
    VAMPIRE = 1,
    HUNTER,
    WIZARD
};

string styleToString(CharacterStyle style) {
    switch (style) {
    case VAMPIRE: return "Vampire";
    case HUNTER:  return "Hunter";
    case WIZARD:  return "Wizard";
    default:      return "Unknown";
    }
}

// ================= WALK SESSION STRUCT =================
struct WalkSession {
    int steps = 0;
    double minutes = 0.0;
    string idea = "";
    CharacterStyle style = VAMPIRE;
};

// ================= BASE CLASS =================
class Activity {
protected:
    string idea;
    int duration;
    CharacterStyle style;

public:
    Activity(string i = "", int d = 0, CharacterStyle s = VAMPIRE)
        : idea(i), duration(d), style(s) {
    }

    virtual ~Activity() {}

    string getIdea() const { return idea; }
    int getDuration() const { return duration; }
    CharacterStyle getStyle() const { return style; }

    virtual void print() const {
        cout << "Idea: " << idea << endl;
        cout << "Duration: " << duration << " minutes" << endl;
        cout << "Style: " << styleToString(style) << endl;
    }

    // Polymorphic stream support
    virtual void toStream(ostream& out) const {
        out << idea << " | "
            << duration << " min | "
            << styleToString(style);
    }
};

// ================= DERIVED CLASS #1 =================
class WalkingActivity : public Activity {
private:
    int steps;

public:
    WalkingActivity(string i = "", int d = 0,
        CharacterStyle s = VAMPIRE, int st = 0)
        : Activity(i, d, s), steps(st) {
    }

    int getSteps() const { return steps; }

    // Equality operator
    bool operator==(const WalkingActivity& other) const {
        return idea == other.idea &&
            duration == other.duration &&
            steps == other.steps;
    }

    void print() const override {
        Activity::print();
        cout << "Steps: " << steps << endl;
    }

    void toStream(ostream& out) const override {
        Activity::toStream(out);
        out << " | Steps: " << steps;
    }
};

// ================= DERIVED CLASS #2 =================
class StyledActivity : public Activity {
private:
    CharacterStyle activityStyle;

public:
    StyledActivity(string i = "", int d = 0,
        CharacterStyle s = VAMPIRE)
        : Activity(i, d, s), activityStyle(s) {
    }

    CharacterStyle getActivityStyle() const {
        return activityStyle;
    }

    void toStream(ostream& out) const override {
        Activity::toStream(out);
        out << " | Activity Style: "
            << styleToString(activityStyle);
    }
};

// ================= STREAM INSERTION =================
ostream& operator<<(ostream& out, const Activity& activity) {
    activity.toStream(out);
    return out;
}

// ================= ORIGINAL STEPTRACKER (WEEK 05) =================
class StepTracker {
private:
    WalkSession sessions[MAX_SESSIONS];
    int count;

public:
    StepTracker() : count(0) {}

    bool addSession(const WalkSession& s) {
        if (count >= MAX_SESSIONS ||
            s.steps <= 0 || s.minutes <= 0)
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
            total += sessions[i].steps /
            sessions[i].minutes;

        return total / count;
    }
};

// ================= FUNCTION TEMPLATE =================
template <typename T>
T getMax(T a, T b) {
    return (a > b) ? a : b;
}

// ================= TEMPLATE CONTAINER (WEEK 06) =================
template <typename T>
class ActivityContainer {
private:
    T** items;
    int size;
    int capacity;

    void resize() {
        capacity *= 2;
        T** temp = new T * [capacity];

        for (int i = 0; i < size; i++)
            temp[i] = items[i];

        delete[] items;
        items = temp;
    }

public:
    ActivityContainer(int cap = 5)
        : size(0), capacity(cap) {
        items = new T * [capacity];
    }

    ~ActivityContainer() {
        for (int i = 0; i < size; i++)
            delete items[i];
        delete[] items;
    }

    int getSize() const { return size; }

    // operator +=
    ActivityContainer& operator+=(T* item) {
        if (size == capacity)
            resize();

        this->items[size++] = item;  // explicit this
        return *this;
    }

    // operator -=
    ActivityContainer& operator-=(int index) {
        if (index < 0 || index >= size)
            return *this;

        delete items[index];

        for (int i = index; i < size - 1; i++)
            items[i] = items[i + 1];

        size--;
        return *this;
    }

    // operator []
    T* operator[](int index) {
        if (index < 0 || index >= size)
            return nullptr;

        return items[index];
    }
};

// ================= ORIGINAL DOCTESTS =================
TEST_CASE("Steps per minute calculation") {
    WalkSession s{ 300, 30.0, "", VAMPIRE };
    CHECK((s.steps / s.minutes) == doctest::Approx(10.0));
}

TEST_CASE("StepTracker adds session") {
    StepTracker tracker;
    CHECK(tracker.addSession({ 1000, 20, "", HUNTER }));
}

TEST_CASE("Total steps calculation") {
    StepTracker tracker;
    tracker.addSession({ 1000, 20, "", VAMPIRE });
    tracker.addSession({ 2000, 40, "", HUNTER });
    CHECK(tracker.getTotalSteps() == 3000);
}

// ================= NEW WEEK 06 TESTS =================
TEST_CASE("WalkingActivity equality") {
    WalkingActivity a("Test", 20, VAMPIRE, 1000);
    WalkingActivity b("Test", 20, VAMPIRE, 1000);
    CHECK(a == b);
}

TEST_CASE("Stream operator works polymorphically") {
    Activity* a = new WalkingActivity("Walk", 30, HUNTER, 2000);
    ostringstream out;
    out << *a;
    CHECK(out.str().find("Steps") != string::npos);
    delete a;
}

TEST_CASE("ActivityContainer add and size") {
    ActivityContainer<Activity> container;
    container += new WalkingActivity("Walk", 10, VAMPIRE, 500);
    CHECK(container.getSize() == 1);
}

TEST_CASE("ActivityContainer indexing") {
    ActivityContainer<Activity> container;
    container += new WalkingActivity("Walk", 10, VAMPIRE, 500);
    CHECK(container[0] != nullptr);
}

TEST_CASE("ActivityContainer remove") {
    ActivityContainer<Activity> container;
    container += new WalkingActivity("A", 10, VAMPIRE, 500);
    container += new WalkingActivity("B", 20, HUNTER, 1000);

    container -= 0;

    CHECK(container.getSize() == 1);
    CHECK(container[0]->getIdea() == "B");
}

TEST_CASE("Function template works") {
    CHECK(getMax(3, 7) == 7);
    CHECK(getMax(2.5, 5.1) == doctest::Approx(5.1));
}