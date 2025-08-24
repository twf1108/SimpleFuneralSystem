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

vector<Event> events;
const string FILE_NAME = "assignment.txt";


// output vector (1 for paid, 2 for unpaid , else all)
void vectorLoop(int typeOutput, string title = "", vector<Event>* records = nullptr) {
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

// Check the input in the range ? and output directly
void vectorLoopAndselectionInput(int typeOutput, string title = "", vector<Event>* records = nullptr) {
    int min = 1, max = 0, selection = 0;

    do {
        vectorLoop(typeOutput, title, records);

        max = (*records).size();

        cout << format("Please enter between {}-{}: ", min, max);
        cin >> selection;

        if (selection < min || selection > max) {
            system("cls");
            cout << "Invalid selection" << endl;
        }
    } while (selection < min || selection > max);
}

// ===== File handling =====
void loadEvents() {
    events.clear();
    ifstream file(FILE_NAME);
    if (!file.is_open()) return;

    string line;
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
    file.close();
}

void saveEvents() {
    ofstream file(FILE_NAME);
    for (auto& e : events) {
        file << e.customerName << ","
            << e.deceasedName << ","
            << e.packageName << ","
            << (e.paid ? "1" : "0") << "\n";
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
    int selection = 0;
    system("cls");

    vectorLoopAndselectionInput(1, "[Event Monitoring]\n", &paidEvents);

    system("cls");

}

int main() {

    int choice;
    loadEvents(); // Load from file when program starts

    do {
        system("cls"); // Use "clear" for Mac/Linux

        cout << "=====================================\n";
        cout << "   Funeral Event Management System   \n";
        cout << "=====================================\n";
        cout << "1. Register Funeral Event\n";
        cout << "2. Payment for an Registered Event\n";
        cout << "3. Monitor a Created Event\n";
        cout << "4. Exit\n";
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