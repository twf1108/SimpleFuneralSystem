#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <format>
using namespace std;

struct Event {
    string customerName;
    string deceasedName;
    string packageName;
    bool paid;
};

struct Date {
    int date, month, year;
};

struct Time {
    int hours, minute;
};

struct Activity {
    string type, from, to;
    double amount;
    string description = "";
    Date date;
    Time time;
};

vector<Event> events;
const string FILE_NAME = "assignment.txt";

void createActivity();
void editActivity();
void viewActivity();
void deleteActivity();
void loopMenu(const vector<string>& menu, int* selection = nullptr, string title = "", bool runInput = false);

// output vector (1 for paid, 2 for unpaid , else all)  // record for record the specific type record
void vectorLoop(int typeOutput = 0, string title = "", vector<Event>* records = nullptr) {
    int count = 0;
    (*records).clear();

    if (title != "") cout << title << endl;

    for (size_t i = 0; i < events.size(); i++) {
        // if only want to output the paid record 
        switch (typeOutput) {
            // output the paid record only 
        case 1:
            if (events[i].paid) {
                cout << ++count << ". " << events[i].deceasedName
                    << " (Customer: " << events[i].customerName << ") - Package: "
                    << events[i].packageName << " - Status: "
                    << "PAID" << "\n";
                if (records) records->push_back(events.at(i));
            }
            break;
            // output the unpaid record only 
        case 2:
            if (!events[i].paid) {
                cout << ++count << ". " << events[i].deceasedName
                    << " (Customer: " << events[i].customerName << ") - Package: "
                    << events[i].packageName << " - Status: "
                    << "UNPAID" << "\n";
                if (records) records->push_back(events.at(i));
            }
            break;
            // output all record
        default:
            cout << i + 1 << ". " << events[i].deceasedName
                << " (Customer: " << events[i].customerName << ") - Package: "
                << events[i].packageName << " - Status: ";
            events[i].paid ? cout << " PAID" : cout << " UNPAID";
            cout << endl;
        }
    }
}

void selctionCheckInput(int min, int max, int& selection, const vector<string>* menu = nullptr, const string* title = nullptr) {
    do {
        cout << format("Please enter between {} - {} (0 for exit): ", min, max);
        cin >> selection;

        if (selection == 0) break;
        if (selection < min || selection > max) {
            system("cls");
            cout << "Invalid selection\n\n";
            loopMenu(*menu, &selection, *title, false);
        }
    } while (selection < min || selection > max);
}

// Check the input in the range ? and output directly                                                     // boolean for continue run the selecion menu?
void vectorLoopAndselectionInput(int typeOutput = 0, string title = "", vector<Event>* records = nullptr, bool* run = nullptr) {
    int min = 0, max = 0, selection = 0;                        

    do {
        vectorLoop(typeOutput, title, records);

        max = (*records).size();

        if (max == 0) {
            cout << "Record not found..." << endl;
            *run = false;
            break;
        }

        cout << format("Please enter between {} - {} (0 for exit): ", min, max);
        cin >> selection;

        if (selection == 0) {
            *run = false;
            break;
        }

        if (selection < min || selection > max) {
            system("cls");
            cout << "Invalid selection" << endl;
        }
    } while (selection < min || selection > max);
}

// loop a menu
void loopMenu(const vector<string> &menu, int* selection, const string title, bool runInput) {
    if (title != "") {
        cout << "=====================================\n";
        cout << "   " << title << "   \n";
        cout << "=====================================\n";
    }
    int i = 0;
    for (const auto& m : menu) {
        cout << ++i << ". " << m << endl;
    }
    if (runInput) selctionCheckInput(1, menu.size(), *selection, &menu, &title);
}

// ===== File handling ===== (allow to read paid, unpaid or all record)
void loadEvents(vector<string>* lines = nullptr, const string& filename = FILE_NAME) {
    events.clear();
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    if(filename == FILE_NAME)
        while (getline(file, line)) {
            stringstream ss(line);
            Event e;
            string paidStr;
            getline(ss, e.customerName, ',');
            getline(ss, e.deceasedName, ',');
            getline(ss, e.packageName, ',');
            getline(ss, paidStr, ',');
            e.paid = (paidStr == "1");
            events.push_back(e);
        }
    else {
        lines->clear();
        if (!file) {
            cerr << "Error: cannot open file " << filename << endl;
            return;
        }

        while (getline(file, line)) {
            lines->push_back(line);
        }
    }

    file.close();
}

void saveEvents(const string &filename = FILE_NAME, vector<string> &lines) {
    ofstream file(filename);
    
    for (auto& e : events) {
        file << e.customerName << ","
            << e.deceasedName << ","
            << e.packageName << ","
            << (e.paid ? "1" : "0") << "\n";
    }

    file.close();
}

void saveEvents(vector<string>& lines, const string& filename) {
    ofstream file(filename);
    for (auto& line : lines) {
        file << line << "\n";
    }
    file.close();
}

// ===== Functions =====
void eventRegistration() {
    Event e;
    cin.ignore();
    cout << "\n[Event Registration]\n";
    cout << "Enter Customer Name: ";
    getline(cin, e.customerName);
    cout << "Enter Deceased's Name: ";
    getline(cin, e.deceasedName);
    cout << "Enter Package Name (Basic/Standard/Premium): ";
    getline(cin, e.packageName);
    e.paid = false;

    events.push_back(e);
    saveEvents();
    cout << "\nEvent registered successfully for " << e.deceasedName << ".\n";
}

void eventPayment() {
    cout << "\n[Event Payment]\n";
    cout << "Select an event to pay for:\n";

    for (size_t i = 0; i < events.size(); i++) {
        if (!events[i].paid)
            cout << i + 1 << ". Deceased Name:  " << events[i].deceasedName
            << " (Register Customer: " << events[i].customerName << ")";

        cout << "\n";
    }

    int choice;
    cout << "Enter event number: ";
    cin >> choice;

    if (choice < 1 || choice >(int)events.size()) {
        cout << "Invalid choice.\n";
        return;
    }

    Event& selected = events[choice - 1];
    if (selected.paid) {
        cout << "This event is already paid.\n";
    }
    else {
        cout << "Processing payment for " << selected.deceasedName << "...\n";
        selected.paid = true;
        saveEvents();
        cout << "Payment completed.\n";
    }
}

void eventMonitoring() {
    vector<Event> paidEvents;
    vector<string> menu = { "Create Activity","View Activity","Edit Activity","Delete Activity" };
    int selection = 0;
    bool run = true;

    while (selection == 0) {
        system("cls");
        // call function to output the paid record and let user select 
        vectorLoopAndselectionInput(1, "[Event Monitoring]\n", &paidEvents, &run);

        if (run) {
            cout << endl;
            system("cls");
            loopMenu(menu, &selection, "Monitor a Created Event", true); // select on specific user

            switch (selection) {
            case 1:
                createActivity();
                break;
            case 2:
                viewActivity();
                break;
            case 3:
                editActivity();
                break;
            case 4:
                deleteActivity();
                break;
            }
        }
        else break;
    }
}

void createActivity() {
    vector<string> lines;
    string filename = "activity.txt";
    Activity activity;

    cout << "Initiator: ";
    getline(cin, activity.from);

    cout << "Invitees: ";
    getline(cin, activity.to);

    cout << "Type of Activity: ";
    getline(cin, activity.type);

    cout << "Amount of Activity: ";
    cin >> activity.amount;
    cin.ignore();

    cout << "Description: ";
    getline(cin, activity.description);
    cin.ignore();

    cout << "Date (dd mm yy): ";
    cin >> activity.date.date >> activity.date.month >> activity.date.year;

    cout << "Date (hh mm): ";
    cin >> activity.time.hours >> activity.time.minute;

    stringstream ss;
    ss << activity.from << ","
        << activity.to << ","
        << activity.type << ","
        << activity.amount << ","
        << activity.description << ","
        << activity.date.date << " " << activity.date.month << " " << activity.date.year << ","
        << activity.time.hours << " " << activity.time.minute;


    lines.push_back(ss.str());
    //loadEvents(&lines, filename);

}
//string type, from, to;
//double amount;
//string description = "";
//DateTime dateTime;

void viewActivity() {
    cout << "view";
}
void editActivity() {
    cout << "edit";
}
void deleteActivity() {
    cout << "delete";
}


int main() {

    int choice;
    loadEvents(); // Load from file when program starts

    vector<string> menu = { "Register Funeral Event", "Payment for an Registered Event", "Monitor a Created Event", "Exit"};
    do {
        system("cls"); // Use "clear" for Mac/Linux

        loopMenu(menu, nullptr ,"Funeral Event Management System");

        cout << "-------------------------------------\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: eventRegistration(); break;
        case 2:
            if (events.empty()) cout << "\n[ERROR] No events registered yet.\n";
            else eventPayment();
            break;
        case 3:
            if (events.empty()) cout << "\n[ERROR] No events registered yet.\n";
            else eventMonitoring();
            break;
        case 4:
            saveEvents();
            cout << "Exiting system... Goodbye!\n";
            break;
        default:
            cout << "Invalid choice! Please try again.\n";
            break;
        }

        if (choice != 4) {
            cout << "\nPress Enter to return to the main menu...";
            cin.ignore();
            cin.get();
        }

    } while (choice != 4);

    return 0;
}