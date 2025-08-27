#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <format>
using namespace std;

struct Date {
    int year;
    int month;
    int date;
};

struct Deceased {
    string deceasedName;
    int age;
    Date deadDay;
};

struct Package {
    string name;
    string detail;
    double price;
};

struct AddOn {
    string name;
    double price;
};

struct Event {
    string customerName;
    Deceased deceased;
    Date date;
    Package package;
    AddOn addOn;
    int totalGuest;
    double basePrice;
    double totalPrice = 0.0;
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
                cout << ++count << ". " << events[i].deceased.deceasedName
                    << " (Customer: " << events[i].customerName << ") - Package: "
                    << events[i].package.name << " - Status: "
                    << "PAID" << "\n";
                if (records) records->push_back(events.at(i));
            }
            break;
            // output the unpaid record only 
        case 2:
            if (!events[i].paid) {
                cout << ++count << ". " << events[i].deceased.deceasedName
                    << " (Customer: " << events[i].customerName << ") - Package: "
                    << events[i].package.name << " - Status: "
                    << "UNPAID" << "\n";
                if (records) records->push_back(events.at(i));
            }
            break;
            // output all record
        default:
            cout << i + 1 << ". " << events[i].deceased.deceasedName
                << " (Customer: " << events[i].customerName << ") - Package: "
                << events[i].package.name << " - Status: ";
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
    if (filename == FILE_NAME) {
        while (getline(file, line)) {
            stringstream ss(line);
            Event e;
            string paidStr, priceStr, addOnPriceStr, totalPrice, totalGuest, basePrice;

            getline(ss, e.customerName, ',');
            getline(ss, e.deceased.deceasedName, ',');

            string ageStr;
            getline(ss, ageStr, ',');
            e.deceased.age = ageStr.empty() ? 0 : stoi(ageStr);

            // Deceased death date
            string y, m, d;
            getline(ss, y, ','); e.deceased.deadDay.year = stoi(y);
            getline(ss, m, ','); e.deceased.deadDay.month = stoi(m);
            getline(ss, d, ','); e.deceased.deadDay.date = stoi(d);

            // Event date
            getline(ss, y, ','); e.date.year = stoi(y);
            getline(ss, m, ','); e.date.month = stoi(m);
            getline(ss, d, ','); e.date.date = stoi(d);

            // Package
            getline(ss, e.package.name, ',');
            getline(ss, priceStr, ',');
            e.package.price = priceStr.empty() ? 0.0 : stod(priceStr);

            // AddOn
            getline(ss, e.addOn.name, ',');
            getline(ss, addOnPriceStr, ',');
            e.addOn.price = addOnPriceStr.empty() ? 0.0 : stod(addOnPriceStr);

            getline(ss, totalGuest, ',');
            e.totalGuest = totalGuest.empty() ? 0.0 : stoi(totalGuest);

            getline(ss, basePrice, ',');
            e.basePrice = basePrice.empty() ? 0.0 : stod(basePrice);
            getline(ss, totalPrice, ',');
            e.addOn.price = totalPrice.empty() ? 0.0 : stod(addOnPriceStr);

            // Paid flag
            getline(ss, paidStr, ',');
            e.paid = (paidStr == "1");

            e.totalPrice = e.package.price + e.addOn.price;

            events.push_back(e);
        }
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
            << e.deceased.deceasedName << ","
            << e.deceased.age << ","
            << e.deceased.deadDay.year << "," << e.deceased.deadDay.month << "," << e.deceased.deadDay.date << ","
            << e.date.year << "," << e.date.month << "," << e.date.date << ","
            << e.package.name << "," << e.package.price << ","
            << e.addOn.name << "," << e.addOn.price << ","
            << e.totalGuest << "," << e.basePrice << ","
            << e.totalPrice << ","
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
// ===== Booking =====
void addOn(Event& e) {
    vector<AddOn> addons = {
        {"Flower Arrangement", 150.0},
        {"Catering Service", 300.0},
        {"Memorial Book", 50.0},
        {"Music Service", 200.0},
        {"Grave Marker", 400.0}
    };

    cout << "Funeral Add-on Selection Menu\n";
    cout << "------------------------------\n";

    for (int i = 0; i < addons.size(); i++) {
        cout << i + 1 << ". " << addons[i].name << " - RM " << addons[i].price << endl;
    }

    int choice;
    cout << "Enter choice (0 for none): ";
    cin >> choice;

    if (choice >= 1 && choice <= addons.size()) {
        e.addOn = addons.at(choice - 1);
    }
    else {
        e.addOn = { "No Add-on", 0.0 };
    }
}

void selectPackage(Event& e) {
    e.basePrice = e.totalGuest * 30;

    vector<Package> packages = {
        {"Basic Funeral Package", "Professional service fees ...", 3800.00},
        {"Standard Funeral Package", "Includes Basic + embalming ...", 5500.00},
        {"Premium Funeral Package", "Includes Standard + premium casket ...", 9500.00}
    };

    for (int i = 0; i < packages.size(); i++) {
        cout << i + 1 << ". " << packages[i].name << endl
            << "Detail: " << packages[i].detail << endl
            << " - RM" << packages[i].price << endl;
    }

    int choice;
    cout << "Choose package (1-" << packages.size() << "): ";
    cin >> choice;
    e.package = packages.at(choice - 1);   // ✅ fix

    string addOnChoice;
    cout << "Do you want to add on (Yes or No): ";
    cin.ignore();
    getline(cin, addOnChoice);

    if (addOnChoice == "Yes") {
        addOn(e);
    }
    else {
        e.addOn = { "No Add-on", 0.0 };
    }

    e.totalPrice = e.package.price + e.addOn.price + e.basePrice;
}

void eventRegistration() {
    Event e;

    cin.ignore();
    cout << "\n[Event Registration]\n";

    // Customer info
    cout << "Enter Customer Name: ";
    getline(cin, e.customerName);

    // Deceased info
    cout << "Enter Deceased's Name: ";
    getline(cin, e.deceased.deceasedName);

    cout << "Enter Deceased's Age: ";
    cin >> e.deceased.age;
    while (cin.fail()) {
        cin.ignore();
        cout << "Invalid input! Please enter an integer: " << endl;
        cin >> e.deceased.age;
    };


    cout << "Enter Date of Death (YYYY MM DD): ";
    cin >> e.deceased.deadDay.year >> e.deceased.deadDay.month >> e.deceased.deadDay.date;

    // Funeral Event Date
    cout << "Enter Funeral Event Date (YYYY MM DD): ";
    cin >> e.date.year >> e.date.month >> e.date.date;

    cout << "Enter total guests: ";
    cin >> e.totalGuest;
    while (cin.fail()) {
        cin.ignore();
        cout << "Invalid input! Please enter an integer: " << endl;
        cin >> e.deceased.age;
    };

    selectPackage(e);

    e.paid = false;

    events.push_back(e);

    cout << "\nEvent successfully registered!\n";

    return;

}

// ===== Payment =====
void eventPayment() {
    cout << "\n[Event Payment]\n";
    cout << "Select an event to pay for:\n";

    for (size_t i = 0; i < events.size(); i++) {
        if (!events[i].paid)
            cout << i + 1 << ". Deceased Name:  " << events[i].deceased.deceasedName
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
        cout << "Processing payment for " << selected.deceased.deceasedName << "...\n";
        selected.paid = true;
        saveEvents();
        cout << "Payment completed.\n";
    }
}

// ===== Monitoring =====
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

    int choice = 0;
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