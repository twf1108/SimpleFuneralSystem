#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <regex>
#include <algorithm>
#include <thread>
#include <limits>
#include <random>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

struct Date {
    int date = 0, month = 0, year = 0;
};

struct Time {
    int hours = 0, minute = 0;
};

struct Activity {
    string type = "", from = "", to = "";
    double amount = 0.0;
    string description = "";
    Date date;
    Time time;
};

struct Customer {
    string customerIC = "";
    string customerName = "";
    string customerHpNo = "";
};

struct Account {
    Customer customer;
    string password = "";
};

struct Deceased {
    string deceasedName = "";
    int age = 0;
    Date deadDay;
};

struct Package {
    string name = "";
    string detail = "";
    double price = 0.0;
};

struct AddOn {
    string name = "";
    double price = 0.0;
};

struct Event {
    Customer customer;
    Deceased deceased;
    Date date;
    Package package;
    AddOn addOn;
    int totalGuest = 0;
    double basePrice = 0.0;
    double totalPrice = 0.0;
    bool paid = false;
};

// --- Function Prototypes ---
void registerAcc();
Account loginAcc();
void accountManagement();
void afterLogin(Account& acc, vector<Event>& events);

// Event Management
void eventInput(Account& acc, vector<Event>& events);
void readEvent(Account& acc, const vector<Event>& events);
void updateEvent(Account& acc, vector<Event>& events);
void delEvent(Account& acc, vector<Event>& events);
void eventRegistration(Account& acc, vector<Event>& events);
void selectPackage(Event& e);
void addOn(Event& e);

// Payment
void eventPayment(Account& acc, vector<Event>& events);

// Activity Monitoring
void eventMonitoring(Account& acc, vector<Event>& events);
void createActivity(const Event& event);
void viewActivity(const string& filename, const Event& event);
void editActivity(const string& filename, const Event& event);
void deleteActivity(const string& filename, const Event& event);

// General Helpers & I/O
void checkInt(int& i);
void checkDouble(double& d);
bool isDeathDateValid(int year, int month, int day);
void inputFuneralDate(Event& e);
string inputIC();
string inputHp();
void inputDeathDate(Event& e);
void loopMenu(const vector<string>& menu, int* selection = nullptr, string title = "", bool runInput = false);
vector<Event> getUnpaidEventsByIC(string IC, const vector<Event>& events);
void loadEvents(vector<Event>& events, vector<string>* lines = nullptr, const string& filename = "assignment.txt");
void saveEvents(const vector<Event>& events);
void saveEvents(const vector<string>& lines, const string& filename);
static void overwriteFile(const string& filename, const vector<string>& lines);
void exitProgram();
static int selectActivityIndex(const string& filename, const Event& event, vector<string>& lines);

// Payment-specific helpers
string selectPaymentMethod();
string getCurrentDateTime();
string generateInvoiceNumber();
bool isValidCardNumber(const string& cardNumber);
bool isValidExpiryDate(int month, int year);
bool isValidCVV(const string& cvv, const string& cardType);
string detectCardType(const string& cardNumber);
void getCardDetails(string& cardNumber, string& expiryMonth, string& expiryYear, string& cvv, string& cardHolderName);
void printReceipt(const Event& event, const string& paymentMethod, const string& transactionId, const string& invoiceNumber);


// ===== Validation =====
void checkInt(int& i) {
    while (true) {
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter an integer: ";
            cin >> i;
        }
        else {
            break;
        }
    };
}

void checkDouble(double& d) {
    while (true) {
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number: ";
            cin >> d;
        }
        else {
            break;
        }
    }
}

bool isDeathDateValid(int year, int month, int day) {
    auto now = system_clock::now();
    auto time_t = system_clock::to_time_t(now);
    tm tm;
    localtime_s(&tm, &time_t);

    int currentYear = tm.tm_year + 1900;
    int currentMonth = tm.tm_mon + 1;
    int currentDay = tm.tm_mday;

    if (year < 1900 || year > currentYear) {
        return false;
    }

    if (month < 1 || month > 12) {
        return false;
    }

    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        daysInMonth[2] = 29;
    }

    if (day < 1 || day > daysInMonth[month]) {
        return false;
    }

    if (year > currentYear ||
        (year == currentYear && month > currentMonth) ||
        (year == currentYear && month == currentMonth && day > currentDay)) {
        return false;
    }

    return true;
}
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <limits>

// Define your Event structure here, as it's not fully provided
// struct Date {
//     int year;
//     int month;
//     int date;
// };
// struct Event {
//     Date date;
//     // other event properties...
// };

using namespace std;

void inputFuneralDate(Event& e) {
    int y, m, d;
    string line;

    // Clear the input buffer once before the loop
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Get current date for comparison
    time_t now = time(0);
    tm currentTime; // Declare a tm struct to store the time
    localtime_s(&currentTime, &now); // Use the secure version

    int currentYear = currentTime.tm_year + 1900;
    int currentMonth = currentTime.tm_mon + 1;
    int currentDay = currentTime.tm_mday;

    while (true) {
        cout << "Enter Funeral Date (YYYY MM DD) or 0 0 0 to cancel: ";
        getline(cin, line);
        stringstream ss(line);

        // Try to extract three integers
        if (!(ss >> y >> m >> d)) {
            cout << "Invalid input format! Please enter numbers only.\n";
            continue;
        }

        // Check for cancellation
        if (y == 0 && m == 0 && d == 0) {
            e.date.year = 0;
            e.date.month = 0;
            e.date.date = 0;
            return;
        }

        // Check if there are any remaining non-whitespace characters
        string remaining;
        if (ss >> remaining) {
            cout << "Invalid input format! Please enter numbers only.\n";
            continue;
        }

        // Date validation logic
        // Check if year is reasonable (not too far in past/future)
        if (y < 1900 || y > 2100) {
            cout << "Invalid year! Please enter a year between 1900 and 2100.\n";
            continue;
        }

        // Check if month is valid
        if (m < 1 || m > 12) {
            cout << "Invalid month! Please enter a month between 1 and 12.\n";
            continue;
        }

        // Days in each month (index 0 = January)
        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        // Check for leap year
        bool isLeapYear = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        if (isLeapYear && m == 2) {
            daysInMonth[1] = 29; // February has 29 days in leap year
        }

        // Check if day is valid for the given month
        if (d < 1 || d > daysInMonth[m - 1]) {
            cout << "Invalid day! ";
            if (m == 2 && isLeapYear) {
                cout << "February " << y << " has 29 days.\n";
            }
            else {
                cout << "Month " << m << " has " << daysInMonth[m - 1] << " days.\n";
            }
            continue;
        }

        // Check if the date is strictly in the future (after today)
        bool isFutureDate = false;
        if (y > currentYear) {
            isFutureDate = true;
        }
        else if (y == currentYear) {
            if (m > currentMonth) {
                isFutureDate = true;
            }
            else if (m == currentMonth) {
                if (d > currentDay) {  // Changed from >= to > to disallow today
                    isFutureDate = true;
                }
            }
        }

        if (!isFutureDate) {
            cout << "Invalid date! Please input a future date (after today).\n";
            continue;
        }

        // All validations passed - store the date
        e.date.year = y;
        e.date.month = m;
        e.date.date = d;
        cout << "Funeral date set to: " << d << "/" << m << "/" << y << endl;
        break; // Exit the loop on valid input
    }

}
string inputIC() {
    string ic;
    cin.ignore(); // Clear any leftover newline characters from previous input

    while (true) {
        cout << "Enter IC (e.g., 990101-01-1234) or 0 to cancel: ";
        getline(cin, ic);

        // Check if the user wants to cancel
        if (ic == "0") {
            return "0";
        }

        // Remove hyphens and spaces for validation
        string tempIC = ic;
        tempIC.erase(remove(tempIC.begin(), tempIC.end(), '-'), tempIC.end());
        tempIC.erase(remove_if(tempIC.begin(), tempIC.end(), ::isspace), tempIC.end());

        // Validate the cleaned IC string
        if (tempIC.length() != 12) {
            cout << "Invalid IC. Please enter exactly 12 digits.\n";
            continue; // Continue the loop to ask for input again
        }

        bool allDigits = true;
        for (char const& c : tempIC) {
            if (!isdigit(c)) {
                allDigits = false;
                break;
            }
        }

        if (allDigits) {
            return tempIC; // Return the valid, cleaned IC
        }
        else {
            cout << "Invalid IC. Please enter only digits.\n";
        }
    }
}
string inputHp() {
    bool valid;
    string hpNo;

    while (true) {
        cout << "Enter Customer Phone Number (e.g., 011-2727 5569) or 0 to cancel: ";
        getline(cin, hpNo);

        if (hpNo == "0") {
            return "0";
        }

        string tempHp = hpNo;
        tempHp.erase(remove_if(tempHp.begin(), tempHp.end(), ::isspace), tempHp.end());
        tempHp.erase(remove(tempHp.begin(), tempHp.end(), '-'), tempHp.end());

        valid = true;
        if (tempHp.length() < 10 || tempHp.length() > 11) valid = false;
        else {
            for (size_t i = 0; i < tempHp.length(); i++) {
                if (!isdigit(tempHp[i])) {
                    valid = false;
                    break;
                }
            }
        }

        if (!valid) {
            cout << "Invalid phone number. Please enter in total 10-11 digits.\n";
        }
        else {
            return tempHp;
        }
    }
}
void inputDeathDate(Event& e) {
    int y, m, d;
    string line;

    // Use a single cin.ignore() to handle any previous leftover newline
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (true) {
        cout << "Enter Date of Death (YYYY MM DD) or 0 0 0 to cancel: ";
        getline(cin, line);
        stringstream ss(line);
        ss >> y >> m >> d;

        if (y == 0 && m == 0 && d == 0) {
            e.deceased.deadDay.year = 0;
            e.deceased.deadDay.month = 0;
            e.deceased.deadDay.date = 0;
            return;
        }

        if (ss.fail() || !ss.eof()) {
            cout << "Invalid input format! Please enter numbers only.\n";
            continue;
        }

        if (!isDeathDateValid(y, m, d)) {
            cout << "Invalid date! Must be 1900 - today. Try again (or 0 0 0 to cancel): ";
            continue;
        }

        e.deceased.deadDay.year = y;
        e.deceased.deadDay.month = m;
        e.deceased.deadDay.date = d;
        break;
    }
}

// ==== Loop ====
vector<Event> getUnpaidEventsByIC(string IC, const vector<Event>& events) {
    vector<Event> temp;
    for (const auto& event : events) {
        if (event.customer.customerIC == IC && !event.paid) {
            temp.push_back(event);
        }
    }
    return temp;
}

void vectorLoop(const vector<Event>& events, int typeOutput = 0, string title = "", vector<Event>* records = nullptr) {
    int count = 0;
    if (records) (*records).clear();

    if (title != "") cout << title << endl;

    for (size_t i = 0; i < events.size(); i++) {
        switch (typeOutput) {
        case 1:
            if (events[i].paid) {
                cout << ++count << ". " << events[i].deceased.deceasedName
                    << " (Customer: " << events[i].customer.customerName << ") - Package: "
                    << events[i].package.name << " - Status: "
                    << "PAID" << "\n";
                if (records) records->push_back(events.at(i));
            }
            break;
        case 2:
            if (!events[i].paid) {
                cout << ++count << ". " << events[i].deceased.deceasedName
                    << " (Customer: " << events[i].customer.customerName << ") - Package: "
                    << events[i].package.name << " - Status: "
                    << "UNPAID" << "\n";
                if (records) records->push_back(events.at(i));
            }
            break;
        default:
            cout << i + 1 << ". " << events[i].deceased.deceasedName
                << " (Customer: " << events[i].customer.customerName << ") - Package: "
                << events[i].package.name << " - Status: ";
            events[i].paid ? cout << " PAID" : cout << " UNPAID";
            cout << endl;
        }
    }
}

void selctionCheckInput(int min, int max, int& selection, const vector<string>* menu = nullptr, const string* title = nullptr) {
    do {
        cout << "Please enter between " << min << " - " << max << " (0 for exit): ";
        cin >> selection;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter an integer.\n";
            selection = -1; // Set to invalid value to continue loop
            system("pause");
            system("cls");
            if (menu) loopMenu(*menu, &selection, *title, false);
            continue;
        }
        if (selection == 0) break;
        if (selection < min || selection > max) {
            system("cls");
            cout << "Invalid selection\n\n";
            if (menu) loopMenu(*menu, &selection, *title, false);
        }
    } while (selection < min || selection > max);
}

void vectorLoopAndselectionInput(const vector<Event>& events, int& selection, int typeOutput = 0, string title = "", vector<Event>* records = nullptr, bool* run = nullptr) {
    int min = 0;
    int max = 0;

    do {
        vectorLoop(events, typeOutput, title, records);

        max = int((*records).size());

        if (max == 0) {
            cout << "Record not found..." << endl;
            if (run) *run = false;
            break;
        }

        cout << "Please enter between " << min << " - " << max << " (0 for exit): ";
        cin >> selection;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter an integer.\n";
            system("pause");
            system("cls");
            selection = -1;
            continue;
        }

        if (selection == 0) {
            if (run) *run = false;
            break;
        }

        if (selection < min || selection > max) {
            system("cls");
            cout << "Invalid selection" << endl;
        }
    } while (selection < min || selection > max);
}

void loopMenu(const vector<string>& menu, int* selection, const string title, bool runInput) {
    if (title != "") {
        cout << "=====================================\n";
        cout << "    " << title << "    \n";
        cout << "=====================================\n";
    }
    int i = 0;
    for (const auto& m : menu) {
        cout << ++i << ". " << m << endl;
    }
    if (runInput) selctionCheckInput(1, int(menu.size()), *selection, &menu, &title);
}


// ===== File handling =====
void loadEvents(vector<Event>& events, vector<string>* lines, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    if (filename == "assignment.txt") {
        events.clear();

        while (getline(file, line)) {
            stringstream ss(line);
            Event e;
            string s;

            getline(ss, e.customer.customerIC, ',');
            getline(ss, e.customer.customerName, ',');
            getline(ss, e.customer.customerHpNo, ',');

            getline(ss, e.deceased.deceasedName, ',');
            getline(ss, s, ',');
            e.deceased.age = s.empty() ? 0 : stoi(s);

            getline(ss, s, ','); e.deceased.deadDay.year = stoi(s);
            getline(ss, s, ','); e.deceased.deadDay.month = stoi(s);
            getline(ss, s, ','); e.deceased.deadDay.date = stoi(s);

            getline(ss, s, ','); e.date.year = std::stoi(s);
            getline(ss, s, ','); e.date.month = std::stoi(s);
            getline(ss, s, ','); e.date.date = std::stoi(s);

            getline(ss, e.package.name, ',');
            getline(ss, s, ',');
            e.package.price = s.empty() ? 0.0 : stod(s);

            getline(ss, e.addOn.name, ',');
            getline(ss, s, ',');
            e.addOn.price = s.empty() ? 0.0 : stod(s);

            getline(ss, s, ',');
            e.totalGuest = s.empty() ? 0 : stoi(s);

            getline(ss, s, ',');
            e.basePrice = s.empty() ? 0.0 : stod(s);

            getline(ss, s, ',');
            e.totalPrice = s.empty() ? 0.0 : stod(s);

            getline(ss, s, ',');
            if (s == "1") {
                e.paid = true;
            }
            else {
                e.paid = false;
            }

            events.push_back(e);
        }

    }
    else {
        lines->clear();
        while (getline(file, line)) {
            lines->push_back(line);
        }
    }

    file.close();
}

void saveEvents(const vector<Event>& events) {
    ofstream file("assignment.txt");
    for (const auto& e : events) {
        file << e.customer.customerIC << ","
            << e.customer.customerName << ","
            << e.customer.customerHpNo << ","
            << e.deceased.deceasedName << ","
            << e.deceased.age << ","
            << e.deceased.deadDay.year << "," << e.deceased.deadDay.month << "," << e.deceased.deadDay.date << ","
            << e.date.year << "," << e.date.month << "," << e.date.date << ","
            << e.package.name << "," << e.package.price << ","
            << e.addOn.name << "," << e.addOn.price << ","
            << e.totalGuest << "," << e.basePrice << "," << e.totalPrice << ","
            << (e.paid ? "1" : "0") << "\n";
    }
    file.close();
}

void saveEvents(const vector<string>& lines, const string& filename) {
    ofstream file(filename, ios::app);

    if (!file.is_open()) {
        cerr << "Error: cannot open file " << filename << endl;
        return;
    }

    for (const auto& line : lines) {
        file << line << endl;
    }

    file.close();
}

static void overwriteFile(const string& filename, const vector<string>& lines) {
    ofstream out(filename);
    if (!out.is_open()) {
        cerr << "Error: cannot open file " << filename << endl;
        return;
    }
    for (const auto& l : lines) out << l << '\n';
}
void exitProgram() {
    cout << "Exiting program... Goodbye!\n";
    exit(0);
}

// ===== Account =====
void registerAcc() {
    string ic, name, password, hp;
    string line;

    system("cls");
    cout << "\n[Register Account]\n";
    cout << "Enter 0 at any time to cancel registration\n";

    ic = inputIC();
    if (ic == "0") {
        cout << "Registration cancelled.\n";
        return;
    }

    cout << "Enter your name: ";
    getline(cin, name);
    if (name == "0") {
        cout << "Registration cancelled.\n";
        return;
    }

    hp = inputHp();
    if (hp == "0") {
        cout << "Registration cancelled.\n";
        return;
    }

    cout << "Enter your password: ";
    getline(cin, password);
    if (password == "0") {
        cout << "Registration cancelled.\n";
        return;
    }

    ifstream readFile("accounts.txt");
    ofstream file("accounts.txt", ios::app);

    if (file.is_open()) {
        while (getline(readFile, line)) {
            stringstream ss(line);
            string tempIc, tempName, tempPass, tempHp;
            getline(ss, tempIc, ',');
            getline(ss, tempName, ',');
            getline(ss, tempPass, ',');
            getline(ss, tempHp, ',');
            if (tempIc == ic) {
                cout << "Error: IC already registered.\n";
                file.close();
                readFile.close();
                return;
            }
        }
        file << ic << "," << name << "," << password << "," << hp << "\n";
        cout << "Account registered successfully.\n";
    }
    else {
        cout << "Error: Unable to open accounts.txt\n";
    }
    file.close();
    readFile.close();
}

Account loginAcc() {
    Account acc;
    string icInput, passwordInput;
    string line;

    system("cls");
    cout << "\n[Login Account]\n";

    // The inputIC function already has a cin.ignore() to handle leftover newlines.
    icInput = inputIC();
    if (icInput == "0") {
        cout << "Login cancelled." << endl;
        return acc;
    }

    // Explicitly clear the buffer just in case, before reading the password
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter your password: ";
    getline(cin, passwordInput);
    if (passwordInput == "0") {
        cout << "Login cancelled." << endl;
        return acc;
    }

    ifstream file("accounts.txt");
    if (!file.is_open()) {
        cout << "Error: Unable to open accounts.txt\n";
        return acc;
    }

    bool found = false;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string ic, name, pass, hp;

        getline(ss, ic, ',');
        getline(ss, name, ',');
        getline(ss, pass, ',');
        getline(ss, hp, ',');

        // Trim leading and trailing spaces from the password read from the file.
        // This is a safety measure to handle any extra spaces.
        pass.erase(pass.find_last_not_of(" \n\r\t") + 1);
        pass.erase(0, pass.find_first_not_of(" \n\r\t"));

        if (ic == icInput && pass == passwordInput) {
            acc.customer.customerIC = ic;
            acc.customer.customerName = name;
            acc.customer.customerHpNo = hp;
            acc.password = pass;
            found = true;
            break;
        }
    }

    file.close();

    if (found) {
        cout << "Login successful.\n";
        vector<Event> events;
        loadEvents(events, nullptr, "assignment.txt");
        afterLogin(acc, events);
    }
    else {
        cout << "Login failed: Incorrect IC or password.\n";
    }

    return acc;
}

void accountManagement() {
    vector<string> menu = { "Register Account", "Login Account", "Exit Program" };
    int selection = 0;
    bool run = true;

    while (run) {
        system("cls");
        loopMenu(menu, &selection, "Account Management", true);

        switch (selection) {
        case 1:
            registerAcc();
            break;
        case 2:
            loginAcc();
            break;
        case 3:
            exitProgram();
            break;
        default:
            cout << "Invalid input: Please enter integer between 1 - 3.\n";
            break;
        }

        system("pause");
    }
}

// ===== Registration =====
void addOn(Event& e) {
    system("cls");
    cout << "\n[Add On]\n";
    vector<AddOn> addons = {
        {"Flower Arrangement", 150.0},
        {"Catering Service", 300.0},
        {"Memorial Book", 50.0},
        {"Music Service", 200.0},
        {"Grave Marker", 400.0}
    };

    while (true) {
        cout << "\nFuneral Add-on Selection Menu\n";
        cout << "==============================\n";

        for (size_t i = 0; i < addons.size(); i++) {
            cout << i + 1 << ". " << addons[i].name << " - RM " << addons[i].price << endl;
        }
        cout << "0. No Add-on\n";

        int choice;
        cout << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }

        if (choice == 0) {
            e.addOn = { "No Add-on", 0.0 };
            break;
        }
        else if (choice >= 1 && choice <= static_cast<int>(addons.size())) {
            e.addOn = addons.at(choice - 1);
            break;
        }
        else {
            cout << "Invalid choice! Please try again.\n";
        }
    }
}

void selectPackage(Event& e) {
    system("cls");
    cout << "\n[Select Package]\n";

    vector<Package> packages = {
        {"Basic Funeral Package", "Professional service fees, basic casket, transport", 3800.00},
        {"Standard Funeral Package", "Includes Basic + embalming, premium flowers", 5500.00},
        {"Premium Funeral Package", "Includes Standard + premium casket, memorial service", 9500.00}
    };

    cout << "Available Packages:\n";
    cout << "==================\n";
    for (size_t i = 0; i < packages.size(); i++) {
        cout << i + 1 << ". " << packages[i].name << endl
            << "     Details: " << packages[i].detail << endl
            << "     Price: RM" << fixed << setprecision(2) << packages[i].price << endl << endl;
    }

    int choice;
    do {
        cout << "Choose package (1-" << packages.size() << "): ";
        cin >> choice;
        checkInt(choice);

        if (choice < 1 || choice > static_cast<int>(packages.size())) {
            cout << "Invalid choice. Please select between 1-" << packages.size() << ".\n";
        }
    } while (choice < 1 || choice > static_cast<int>(packages.size()));

    e.package = packages[choice - 1];

    cin.ignore();
    string addOnChoice;
    cout << "Do you want to add an add-on service? (yes/no): ";
    getline(cin, addOnChoice);

    transform(addOnChoice.begin(), addOnChoice.end(), addOnChoice.begin(), ::tolower);

    if (addOnChoice == "yes" || addOnChoice == "y") {
        addOn(e);
    }
    else {
        e.addOn = { "No Add-on", 0.0 };
    }

    e.basePrice = e.totalGuest * 30.0;
    e.totalPrice = e.package.price + e.addOn.price + e.basePrice;

    cout << "\nPrice Breakdown:\n";
    cout << "================\n";
    cout << "Package Price: RM" << fixed << setprecision(2) << e.package.price << "\n";
    cout << "Add-on Price:  RM" << fixed << setprecision(2) << e.addOn.price << "\n";
    cout << "Base Price:    RM" << fixed << setprecision(2) << e.basePrice
        << " (" << e.totalGuest << " guests x RM30)\n";
    cout << "Total Price:   RM" << fixed << setprecision(2) << e.totalPrice << "\n";
}

void eventInput(Account& acc, vector<Event>& events) {
    system("cls");
    Event e;
    string line;

    cout << "\n[Event Registration]\n";
    cout << "Enter 0 at any time to cancel registration\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    e.customer.customerIC = acc.customer.customerIC;
    e.customer.customerName = acc.customer.customerName;
    e.customer.customerHpNo = acc.customer.customerHpNo;
    cout << "Enter Deceased's Name(RIP): ";
    getline(cin, e.deceased.deceasedName);

    // Check if the input is "0" to cancel
    if (e.deceased.deceasedName == "0") {
        return;
    }
    // Check if the input is empty and then exit the function
    else if (e.deceased.deceasedName.empty()) {
        cout << "Please enter a name bro may him Rest In Peace" << endl;
        return; // <-- This is the crucial fix
    }

    cout << "Enter Deceased's Age: ";
    string ageInput;
    getline(cin, ageInput);
    if (ageInput == "0") {
        cout << "Registration cancelled.\n";
        return;
    }
    try {
        e.deceased.age = stoi(ageInput);
        if (e.deceased.age < 0) {
            cout << "Age cannot be negative. Registration cancelled.\n";
            return;
        }
    }
    catch (const invalid_argument& ia) {
        cout << "Invalid input for age. Registration cancelled.\n";
        return;
    }
    catch (const out_of_range& oor) {
        cout << "Age out of range. Registration cancelled.\n";
        return;
    }

    inputDeathDate(e);
    if (e.deceased.deadDay.year == 0) {
        cout << "Registration cancelled.\n";
        return;
    }

    inputFuneralDate(e);
    if (e.date.year == 0) {
        cout << "Registration cancelled.\n";
        return;
    }

    cout << "Enter total guests: ";
    string guestInput;
    getline(cin, guestInput);
    if (guestInput == "0") {
        cout << "Registration cancelled.\n";
        return;
    }
    try {
        e.totalGuest = stoi(guestInput);
        if (e.totalGuest < 0) {
            cout << "Number of guests cannot be negative. Registration cancelled.\n";
            return;
        }
    }
    catch (const invalid_argument& ia) {
        cout << "Invalid input for total guests. Registration cancelled.\n";
        return;
    }
    catch (const out_of_range& oor) {
        cout << "Total guests out of range. Registration cancelled.\n";
        return;
    }

    selectPackage(e);

    e.paid = false;

    events.push_back(e);
    saveEvents(events);
    cout << "\nEvent successfully registered!\n";
}

void readEvent(Account& acc, const vector<Event>& events) {
    system("cls");
    bool found = false;
    int count = 0;
    for (size_t i = 0; i < events.size(); i++) {
        if (events[i].customer.customerIC == acc.customer.customerIC) {
            found = true;
            count++;
            cout << "No " << count << ": " << endl;
            cout << "  Deceased Name       :" << events[i].deceased.deceasedName << endl;
            cout << "  Deceased Age        :" << events[i].deceased.age << endl;
            cout << "  Date                :" << events[i].date.year << "-"
                << events[i].date.month << "-"
                << events[i].date.date << endl;
            cout << "  Package             :" << events[i].package.name << endl;
            cout << "  Add-On              :" << events[i].addOn.name
                << "    ($ " << events[i].addOn.price << ")" << endl;
            cout << "  Total Guests        :" << events[i].totalGuest << endl;
            cout << "  Base Price          : RM" << events[i].basePrice << endl;
            cout << "  Total Price         : RM" << events[i].totalPrice << endl;
            cout << "  Paid                :" << (events[i].paid ? "Yes" : "No") << endl;
            cout << "=============================" << endl;
        }
    }
    if (!found) {
        cout << "No event found with this IC.\n";
    }
}

void updateEvent(Account& acc, vector<Event>& events) {
    system("cls");
    string IC = inputIC();
    if (IC == "0") {
        cout << "Update cancelled.\n";
        return;
    }

    vector<int> unpaidIndices;
    vector<Event> unpaidEvents;

    for (size_t i = 0; i < events.size(); i++) {
        if (events[i].customer.customerIC == IC && !events[i].paid) {
            unpaidIndices.push_back(static_cast<int>(i));
            unpaidEvents.push_back(events[i]);
        }
    }

    if (unpaidEvents.empty()) {
        cout << "No unpaid events found with this IC.\n";
        return;
    }

    cout << "\n[Your Unpaid Registered Events]\n";
    for (size_t i = 0; i < unpaidEvents.size(); ++i) {
        cout << i + 1 << ". " << unpaidEvents[i].deceased.deceasedName
            << " - Package: " << unpaidEvents[i].package.name
            << " - Date: " << unpaidEvents[i].date.year << "-"
            << unpaidEvents[i].date.month << "-" << unpaidEvents[i].date.date << "\n";
    }

    int choice;
    cout << "\nChoose event to update (1-" << unpaidEvents.size() << ", 0 to cancel): ";
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input! Update cancelled.\n";
        return;
    }

    if (choice == 0) {
        cout << "Update cancelled.\n";
        return;
    }

    if (choice < 1 || choice > static_cast<int>(unpaidEvents.size())) {
        cout << "Invalid choice.\n";
        return;
    }

    int mainVectorIndex = unpaidIndices[choice - 1];
    Event& e = events[mainVectorIndex];

    bool editing = true;
    while (editing) {
        system("cls");
        cout << "\n[Update Event: " << e.deceased.deceasedName << "]\n";
        cout << "1. Funeral Date (" << e.date.year << "-"
            << setw(2) << setfill('0') << e.date.month << "-"
            << setw(2) << setfill('0') << e.date.date << ")\n";
        cout << "2. Guests (" << e.totalGuest << ")\n";
        cout << "3. Package (" << e.package.name << " - RM" << e.package.price << ")\n";
        cout << "4. Add-On (" << e.addOn.name << " - RM" << e.addOn.price << ")\n";
        cout << "0. Finish Editing\n";
        cout << "Choose field to update: ";
        int opt;
        cin >> opt;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            system("pause");
            continue;
        }

        switch (opt) {
        case 1:
            inputFuneralDate(e);
            break;
        case 2:
            cout << "Current guests: " << e.totalGuest << "\n";
            cout << "Enter new total guests: ";
            cin >> e.totalGuest;
            checkInt(e.totalGuest);
            e.basePrice = e.totalGuest * 30.0;
            e.totalPrice = e.package.price + e.addOn.price + e.basePrice;
            cout << "Updated! New base price: RM" << e.basePrice
                << ", New total: RM" << e.totalPrice << "\n";
            break;
        case 3:
            selectPackage(e);
            break;
        case 4:
            addOn(e);
            e.totalPrice = e.package.price + e.addOn.price + e.basePrice;
            cout << "Add-on updated! New total price: RM" << e.totalPrice << "\n";
            break;
        case 0:
            editing = false;
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
            system("pause");
            break;
        }

        if (editing && opt >= 1 && opt <= 4) {
            cout << "Field updated successfully!\n";
            system("pause");
        }
    }

    saveEvents(events);
    cout << "\nEvent updated successfully!\n";
}

void eventMonitoring(Account& acc, vector<Event>& events) {
    vector<Event> paidEvents;
    vector<string> menu = { "Create Activity", "View Activity", "Edit Activity", "Delete Activity", "Exit" };
    int userSelection = 0;
    int selection = 0;
    bool run = true;

    while (run) {
        system("cls");

        loadEvents(events, nullptr, "assignment.txt");

        paidEvents.clear();
        for (const auto& event : events) {
            if (event.paid && event.customer.customerIC == acc.customer.customerIC) {
                paidEvents.push_back(event);
            }
        }

        if (paidEvents.empty()) {
            cout << "[Event Monitoring]\n\n";
            cout << "No paid events found for monitoring.\n";
            cout << "Events must be paid before they can be monitored.\n";
            return;
        }

        cout << "[Event Monitoring]\n\n";
        cout << "Select a paid event to monitor:\n";
        for (size_t i = 0; i < paidEvents.size(); i++) {
            cout << i + 1 << ". " << paidEvents[i].deceased.deceasedName
                << " (Customer: " << paidEvents[i].customer.customerName
                << ") - Package: " << paidEvents[i].package.name
                << " - Date: " << paidEvents[i].date.year << "-"
                << setw(2) << setfill('0') << paidEvents[i].date.month << "-"
                << setw(2) << setfill('0') << paidEvents[i].date.date << "\n";
        }

        cout << "\nEnter selection (1-" << paidEvents.size() << ", 0 to exit): ";
        cin >> userSelection;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            system("pause");
            continue;
        }

        if (userSelection == 0) {
            run = false;
            break;
        }

        if (userSelection < 1 || userSelection > static_cast<int>(paidEvents.size())) {
            cout << "Invalid selection. Please try again.\n";
            system("pause");
            continue;
        }

        Event selectedEvent = paidEvents[userSelection - 1];

        bool activityMenu = true;
        while (activityMenu) {
            system("cls");
            cout << "Monitoring Event: " << selectedEvent.deceased.deceasedName << "\n";
            cout << "Customer: " << selectedEvent.customer.customerName << "\n";
            cout << "Event Date: " << selectedEvent.date.year << "-"
                << setw(2) << setfill('0') << selectedEvent.date.month << "-"
                << setw(2) << setfill('0') << selectedEvent.date.date << "\n\n";

            loopMenu(menu, &selection, "Monitor Activities", true);

            switch (selection) {
            case 1:
                createActivity(selectedEvent);
                break;
            case 2:
                viewActivity("activity.txt", selectedEvent);
                break;
            case 3:
                editActivity("activity.txt", selectedEvent);
                break;
            case 4:
                deleteActivity("activity.txt", selectedEvent);
                break;
            case 5:
                activityMenu = false;
                break;
            case 0: // Check for 0 as a way to exit from the inner loop
                activityMenu = false;
                run = false; // Also exit the outer loop
                break;
            default:
                cout << "Invalid selection.\n";
                break;
            }

            if (selection != 5 && selection != 0) {
                system("pause");
            }
        }
    }
}

bool isValidCardNumber(const string& cardNumber) {
    int sum = 0;
    bool isSecondDigit = false;
    for (int i = static_cast<int>(cardNumber.length()) - 1; i >= 0; i--) {
        if (!isdigit(cardNumber[i])) return false;
        int digit = cardNumber[i] - '0';
        if (isSecondDigit) {
            digit *= 2;
            if (digit > 9) {
                digit -= 9;
            }
        }
        sum += digit;
        isSecondDigit = !isSecondDigit;
    }
    return (sum % 10 == 0);
}

bool isValidExpiryDate(int month, int year) {
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);
    int currentYear = 1900 + ltm.tm_year;
    int currentMonth = 1 + ltm.tm_mon;

    if (year < currentYear || year > currentYear + 10) return false;
    if (month < 1 || month > 12) return false;
    if (year == currentYear && month < currentMonth) return false;

    return true;
}

bool isValidCVV(const string& cvv, const string& cardType) {
    if (cvv.length() != 3 && cvv.length() != 4) return false;

    for (char c : cvv) {
        if (!isdigit(c)) return false;
    }

    if (cardType == "American Express" && cvv.length() != 4) return false;
    if (cardType != "American Express" && cvv.length() != 3) return false;

    return true;
}

string detectCardType(const string& cardNumber) {
    if (cardNumber.empty()) return "Unknown";

    if (cardNumber.length() >= 2 && (cardNumber.substr(0, 2) == "34" || cardNumber.substr(0, 2) == "37"))
        return "American Express";

    if (cardNumber.length() >= 1 && cardNumber[0] == '4') return "Visa";

    try {
        if (cardNumber.length() >= 2) {
            int firstTwoDigits = stoi(cardNumber.substr(0, 2));
            if (firstTwoDigits >= 51 && firstTwoDigits <= 55) return "Mastercard";
        }
    }
    catch (...) {}

    if (cardNumber.length() >= 4 && cardNumber.substr(0, 4) == "6011")
        return "Discover";
    if (cardNumber.length() >= 3 && (cardNumber.substr(0, 3) >= "644" && cardNumber.substr(0, 3) <= "649"))
        return "Discover";
    if (cardNumber.length() >= 2 && cardNumber.substr(0, 2) == "65")
        return "Discover";

    return "Unknown";
}

string getCurrentDateTime() {
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &ltm);
    return string(buffer);
}

string generateInvoiceNumber() {
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(1000, 9999);
    int randomNum = dist(rng);

    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "INV-%Y%m%d-", &ltm);

    return string(buffer) + to_string(randomNum);
}

string selectPaymentMethod() {
    const int ROWS = 5;
    const int COLS = 4;
    string paymentMethods[ROWS][COLS] = {
        {"CC", "Credit Card", "Pay with Visa/Mastercard/Amex", "1"},
        {"DC", "Debit Card", "Pay with your debit card", "1"},
        {"OB", "Online Banking", "Pay via online banking", "0"},
        {"EW", "E-Wallet", "Pay with e-wallet (Touch'n'Go, GrabPay, etc.)", "0"},
        {"COD", "Cash on Delivery", "Pay in cash at the time of service", "1"}
    };

    while (true) {
        cout << "\nSelect Payment Method:\n";
        cout << "=======================\n";
        for (int i = 0; i < ROWS; i++) {
            cout << i + 1 << ". " << paymentMethods[i][1] << "\n";
        }
        cout << "Enter choice: ";
        int choice;
        cin >> choice;

        if (cin.fail() || choice < 1 || choice > ROWS) {
            cout << "Invalid choice. Please try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            return paymentMethods[choice - 1][0];
        }
    }
}

void getCardDetails(string& cardNumber, string& expiryMonth, string& expiryYear, string& cvv, string& cardHolderName) {
    bool valid = false;
    while (!valid) {
        cout << "Enter Card Number: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, cardNumber);

        cardNumber.erase(remove_if(cardNumber.begin(), cardNumber.end(), ::isspace), cardNumber.end());
        cardNumber.erase(remove(cardNumber.begin(), cardNumber.end(), '-'), cardNumber.end());

        if (!isValidCardNumber(cardNumber)) {
            cout << "Invalid card number. Please try again.\n";
            continue;
        }

        string cardType = detectCardType(cardNumber);
        cout << "Detected Card Type: " << cardType << "\n";

        cout << "Enter Expiry Date (MM YY): ";
        cin >> expiryMonth >> expiryYear;

        try {
            int month = stoi(expiryMonth);
            int year = stoi(expiryYear);
            if (year < 100) year += 2000;
            if (!isValidExpiryDate(month, year)) {
                cout << "Invalid expiry date. Please try again.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
        }
        catch (const invalid_argument& e) {
            cout << "Invalid input! Please enter numbers for month and year.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cout << "Enter CVV: ";
        cin >> cvv;
        if (!isValidCVV(cvv, cardType)) {
            cout << "Invalid CVV. Please try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cout << "Enter Cardholder Name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, cardHolderName);

        valid = true;
    }
}

void printReceipt(const Event& event, const string& paymentMethod, const string& transactionId, const string& invoiceNumber) {
    system("cls");
    cout << "\n=====================================\n";
    cout << "           PAYMENT RECEIPT           \n";
    cout << "=====================================\n";
    cout << "Invoice No. : " << invoiceNumber << "\n";
    cout << "Date        : " << getCurrentDateTime() << "\n";
    cout << "=====================================\n";
    cout << "Customer    : " << event.customer.customerName << "\n";
    cout << "Deceased    : " << event.deceased.deceasedName << "\n";
    cout << "Event Date  : " << event.date.year << "-"
        << setw(2) << setfill('0') << event.date.month << "-"
        << setw(2) << setfill('0') << event.date.date << "\n";
    cout << "=====================================\n";
    cout << "Package     : " << event.package.name << "\n";
    cout << "Add-On      : " << event.addOn.name << "\n";
    cout << "Total Guests: " << event.totalGuest << "\n";
    cout << "=====================================\n";
    cout << "Package Price : RM " << fixed << setprecision(2) << event.package.price << "\n";
    cout << "Add-On Price  : RM " << fixed << setprecision(2) << event.addOn.price << "\n";
    cout << "Base Price    : RM " << fixed << setprecision(2) << event.basePrice << "\n";
    cout << "=====================================\n";
    cout << "Total Paid    : RM " << fixed << setprecision(2) << event.totalPrice << "\n";
    cout << "Payment Method: " << paymentMethod << "\n";
    cout << "Transaction ID: " << transactionId << "\n";
    cout << "=====================================\n";
    cout << "Thank you for using our service.\n\n";
}

void eventPayment(Account& acc, vector<Event>& events) {
    system("cls");
    cout << "\n[Event Payment]\n";

    loadEvents(events, nullptr, "assignment.txt");

    // --- CHANGE 1: Find indices instead of making a copy ---
    // This is more efficient and ensures we modify the original event.
    vector<int> unpaidEventIndices;
    for (int i = 0; i < events.size(); ++i) {
        if (events[i].customer.customerIC == acc.customer.customerIC && !events[i].paid) {
            unpaidEventIndices.push_back(i);
        }
    }

    if (unpaidEventIndices.empty()) {
        cout << "No unpaid events found for this IC.\n";
        return;
    }

    cout << "\nUnpaid Events:\n";
    cout << "=====================================\n";
    for (size_t i = 0; i < unpaidEventIndices.size(); ++i) {
        const Event& e = events[unpaidEventIndices[i]];
        cout << i + 1 << ". "
            << setfill(' ') 
            << setw(40) << left << e.deceased.deceasedName
            << " - RM" << setw(8) << fixed << setprecision(2)
            << e.totalPrice << "\n";
    }
    cout << "=====================================\n";

    int choice;
    cout << "\nSelect event to pay (1-" << unpaidEventIndices.size() << ", 0 to cancel): ";
    cin >> choice;

    if (cin.fail() || choice < 0 || choice > unpaidEventIndices.size()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid selection. Payment cancelled.\n";
        return;
    }

    if (choice == 0) {
        cout << "Payment cancelled.\n";
        return;
    }

    // Get a direct reference to the event in the original vector
    Event& selectedEvent = events[unpaidEventIndices[choice - 1]];

    cout << "\nPayment Details:\n";
    cout << "Event: " << selectedEvent.deceased.deceasedName << "'s funeral\n";
    cout << "Package: " << selectedEvent.package.name << "\n";
    cout << "Add-on: " << selectedEvent.addOn.name << "\n";
    cout << "Total Amount: RM" << fixed << setprecision(2) << selectedEvent.totalPrice << "\n\n";

    string paymentMethod = selectPaymentMethod();
    string cardNumber, expiryMonth, expiryYear, cvv, cardHolderName;

    // --- CHANGE 2: Use a better random number generator for transaction IDs ---
    // Replaced outdated rand() with the modern <random> library for consistency.
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(100000, 999999);
    string transactionId = "TXN" + to_string(distrib(gen));
    string invoiceNumber = generateInvoiceNumber();

    if (paymentMethod == "CC" || paymentMethod == "DC") {
        getCardDetails(cardNumber, expiryMonth, expiryYear, cvv, cardHolderName);
        cout << "\nProcessing " << (paymentMethod == "CC" ? "Credit Card" : "Debit Card") << " payment...\n";

        // --- CHANGE 3: Added safety checks to prevent substr crash ---
        string maskedCard = "****-****-****-XXXX"; // Default value
        if (cardNumber.length() >= 4) {
            maskedCard = "****-****-****-" + cardNumber.substr(cardNumber.length() - 4);
        }

        string yearSuffix = expiryYear; // Default value
        if (expiryYear.length() >= 2) {
            yearSuffix = expiryYear.substr(2);
        }

        cout << "Card: " << maskedCard << "\n";
        cout << "Expiry: " << expiryMonth << "/" << yearSuffix << "\n";
        cout << "Cardholder: " << cardHolderName << "\n";
    }
    else if (paymentMethod == "OB") {
        // This section was already safe, no changes needed
        cout << "Available Banks:\n";
        cout << "1. Maybank\n2. CIMB Bank\n3. Public Bank\n4. RHB Bank\n5. Hong Leong Bank\n";
        int bankChoice;
        cout << "Select bank (1-5): ";
        cin >> bankChoice;
        if (cin.fail() || bankChoice < 1 || bankChoice > 5) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid bank choice. Payment cancelled.\n";
            return;
        }
        vector<string> banks = { "Maybank", "CIMB Bank", "Public Bank", "RHB Bank", "Hong Leong Bank" };
        cout << "Redirecting to " << banks[bankChoice - 1] << " online banking...\n";
    }
    else if (paymentMethod == "EW") {
        // This section was also already safe, no changes needed
        cout << "Available E-Wallets:\n";
        cout << "1. Touch 'n Go eWallet\n2. GrabPay\n3. Boost\n4. ShopeePay\n";
        int ewalletChoice;
        cout << "Select e-wallet (1-4): ";
        cin >> ewalletChoice;
        if (cin.fail() || ewalletChoice < 1 || ewalletChoice > 4) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid e-wallet choice. Payment cancelled.\n";
            return;
        }
        vector<string> ewallets = { "Touch 'n Go eWallet", "GrabPay", "Boost", "ShopeePay" };
        cout << "Redirecting to " << ewallets[ewalletChoice - 1] << " payment...\n";
    }
    else if (paymentMethod == "COD") {
        cout << "You have selected Cash on Delivery.\n";
        cout << "Please prepare RM" << fixed << setprecision(2) << selectedEvent.totalPrice
            << " in cash when the service is delivered.\n";
    }

    cout << "\nProcessing payment";
    for (int i = 0; i < 3; i++) {
        cout << ".";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(800));
    }
    cout << "\n\nPayment processed successfully!\n";

    // --- CHANGE 4: Simplified the update logic ---
    // Since we now have a direct reference, we can update the status easily.
    selectedEvent.paid = true;

    saveEvents(events);
    cout << "Payment status updated successfully!\n";

    string paymentMethodName;
    if (paymentMethod == "CC") paymentMethodName = "Credit Card";
    else if (paymentMethod == "DC") paymentMethodName = "Debit Card";
    else if (paymentMethod == "OB") paymentMethodName = "Online Banking";
    else if (paymentMethod == "EW") paymentMethodName = "E-Wallet";
    else if (paymentMethod == "COD") paymentMethodName = "Cash on Delivery";
    else paymentMethodName = "Unknown";

    printReceipt(selectedEvent, paymentMethodName, transactionId, invoiceNumber);
}

void createActivity(const Event& event) {
    system("cls");
    vector<string> lines;
    string filename = "activity.txt";
    Activity activity;
    string input;

    cout << "=====================================\n";
    cout << "     " << "Create Activity" << "     \n";
    cout << "=====================================\n";
    cout << "Enter 0 at any prompt to cancel activity creation.\n";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Initiator: ";
    getline(cin, activity.from);
    if (activity.from == "0") return;

    cout << "Invitees: ";
    getline(cin, activity.to);
    if (activity.to == "0") return;

    cout << "Type of Activity: ";
    getline(cin, activity.type);
    if (activity.type == "0") return;

    cout << "Amount of Activity: ";
    getline(cin, input);
    if (input == "0") return;
    try {
        activity.amount = stod(input);
    }
    catch (const invalid_argument& e) {
        cout << "Invalid input for amount. Activity creation cancelled.\n";
        return;
    }

    cout << "Description: ";
    getline(cin, activity.description);
    if (activity.description == "0") return;

    cout << "Date (dd mm yy): ";
    getline(cin, input);
    if (input == "0") return;
    stringstream dateSs(input);
    dateSs >> activity.date.date >> activity.date.month >> activity.date.year;
    if (dateSs.fail() || !dateSs.eof()) {
        cout << "Invalid date format. Activity creation cancelled.\n";
        return;
    }

    cout << "Time (hh mm): ";
    getline(cin, input);
    if (input == "0") return;
    stringstream timeSs(input);
    timeSs >> activity.time.hours >> activity.time.minute;
    if (timeSs.fail() || !timeSs.eof()) {
        cout << "Invalid time format. Activity creation cancelled.\n";
        return;
    }

    stringstream ss;
    ss << event.customer.customerName << ","
        << event.deceased.deceasedName << ","
        << event.date.date << " " << event.date.month << " " << event.date.year << ","
        << activity.from << ","
        << activity.to << ","
        << activity.type << ","
        << activity.amount << ","
        << activity.description << ","
        << activity.date.date << " " << activity.date.month << " " << activity.date.year << ","
        << activity.time.hours << " " << activity.time.minute;


    lines.push_back(ss.str());
    saveEvents(lines, filename);
    cout << "\nActivity created successfully!\n";
}

void viewActivity(const string& filename, const Event& event) {
    system("cls");
    cout << "=====================================\n";
    cout << "     " << "View Activity" << "     \n";
    cout << "=====================================\n";

    vector<string> lines;
    vector<Event> dummy;
    loadEvents(dummy, &lines, filename);

    bool found = false;
    int activityCount = 0;

    for (const auto& line : lines) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 3) continue;

        string customerName = tokens[0];
        string deceasedName = tokens[1];
        string eventDate = tokens[2];

        string targetDate = to_string(event.date.date) + " " +
            to_string(event.date.month) + " " +
            to_string(event.date.year);

        if (customerName == event.customer.customerName &&
            deceasedName == event.deceased.deceasedName &&
            eventDate == targetDate)
        {
            found = true;
            cout << "=====================================\n";
            cout << " Activity " << ++activityCount << " for " << customerName << " - " << deceasedName << "\n";
            cout << "=====================================\n";

            if (tokens.size() >= 10) {
                cout << "Initiator             : " << tokens[3] << endl;
                cout << "Invitees              : " << tokens[4] << endl;
                cout << "Type of Activity      : " << tokens[5] << endl;
                cout << "Amount                : " << tokens[6] << endl;
                cout << "Description           : " << tokens[7] << endl;
                cout << "Activity Date         : " << tokens[8] << endl;
                cout << "Activity Time         : " << tokens[9] << endl;
            }
            else {
                cout << "Warning: Record format is invalid: " << line << endl;
            }
            cout << endl;
        }
    }

    if (!found) {
        cout << "No activities found for " << event.customer.customerName
            << " (" << event.deceased.deceasedName << ")" << endl;
    }
}

static int selectActivityIndex(const string& filename, const Event& event, vector<string>& lines) {
    lines.clear();
    vector<string> dummy;
    vector<Event> emptyEvents;
    loadEvents(emptyEvents, &lines, filename);

    vector<int> matched;
    const string targetDate =
        to_string(event.date.date) + " " +
        to_string(event.date.month) + " " +
        to_string(event.date.year);

    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) continue;

        stringstream ss(lines[i]);
        string cName, dName, eDate;
        if (!getline(ss, cName, ',')) continue;
        if (!getline(ss, dName, ',')) continue;
        if (!getline(ss, eDate, ',')) continue;

        if (cName == event.customer.customerName &&
            dName == event.deceased.deceasedName &&
            eDate == targetDate) {
            matched.push_back(static_cast<int>(i));
        }
    }

    if (matched.empty()) {
        cout << "No matching activities for this event in " << filename << endl;
        return -1;
    }

    cout << "\nMatching activities (" << matched.size() << "):\n";
    for (size_t j = 0; j < matched.size(); ++j) {
        stringstream ss(lines[matched[j]]);
        string cName, dName, eDate, initiator, invitee, type;
        getline(ss, cName, ',');
        getline(ss, dName, ',');
        getline(ss, eDate, ',');
        getline(ss, initiator, ',');
        getline(ss, invitee, ',');
        getline(ss, type, ',');

        cout << j + 1 << ") Type: " << type << " | Initiator: " << initiator << '\n';
    }

    cout << "Select an activity (1-" << matched.size() << ", 0 to cancel): ";
    int choice;
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input! Please enter a number.\n";
        return -1;
    }

    if (choice == 0) return -1;
    if (choice < 1 || choice > static_cast<int>(matched.size())) {
        cout << "Invalid choice.\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return -1;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return matched[choice - 1];
}

void editActivity(const string& filename, const Event& event) {
    system("cls");
    cout << "=====================================\n";
    cout << "     " << "Edit Activity" << "     \n";
    cout << "=====================================\n";
    vector<string> lines;
    int idx = selectActivityIndex(filename, event, lines);
    if (idx < 0) return;

    vector<string> tokens;
    {
        stringstream ss(lines[idx]);
        string tok;
        while (getline(ss, tok, ',')) tokens.push_back(tok);
    }

    if (tokens.size() < 10) {
        cout << "Malformed record, cannot edit: " << lines[idx] << '\n';
        return;
    }

    auto editField = [&](size_t fieldIndex, const string& label) {
        cout << label << " [" << tokens[fieldIndex] << "]: ";
        string in;
        getline(cin, in);
        if (!in.empty()) tokens[fieldIndex] = in;
        };

    cout << "\n=== Edit Activity Fields (press Enter to keep original) ===\n";
    editField(3, "Initiator");
    editField(4, "Invitees");
    editField(5, "Type");
    editField(6, "Amount");
    editField(7, "Description");
    editField(8, "Activity Date (dd mm yy)");
    editField(9, "Activity Time (hh mm)");

    stringstream os;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i) os << ',';
        os << tokens[i];
    }
    lines[idx] = os.str();

    overwriteFile(filename, lines);
    cout << "Activity updated.\n";
}

void deleteActivity(const string& filename, const Event& event) {
    system("cls");
    cout << "=====================================\n";
    cout << "     " << "Delete Activity" << "     \n";
    cout << "=====================================\n";

    vector<string> lines;
    int idx = selectActivityIndex(filename, event, lines);
    if (idx < 0) return;

    cout << "Delete this record? \n" << lines[idx] << "\nConfirm (y/n): ";
    char yn;
    cin >> yn;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (yn != 'y' && yn != 'Y') {
        cout << "Canceled.\n";
        return;
    }
    lines.erase(lines.begin() + idx);
    overwriteFile(filename, lines);
    cout << "Activity deleted.\n";
}


void afterLogin(Account& acc, vector<Event>& events) {
    int choice = 0;

    vector<string> menu = { "Register Funeral Event", "Update Event", "Payment for an Registered Event", "Monitor a Created Event", "Return to Main Menu" };
    do {
        system("cls");
        loopMenu(menu, nullptr, "Funeral Event Management System");

        cout << "=====================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number.\n";
            system("pause");
            continue;
        }

        switch (choice) {
        case 1:
            eventRegistration(acc, events);
            break;
        case 2:
            if (events.empty()) {
                cout << "\n[ERROR] No events registered yet.\n";
            }
            else {
                updateEvent(acc, events);
            }
            break;
        case 3:
            if (events.empty()) {
                cout << "\n[ERROR] No events registered yet.\n";
            }
            else {
                eventPayment(acc, events);
            }
            break;
        case 4:
            if (events.empty()) {
                cout << "\n[ERROR] No events registered yet.\n";
            }
            else {
                eventMonitoring(acc, events);
            }
            break;
        case 5:
            saveEvents(events);
            cout << "Returning to main menu..." << endl;
            return;
        default:
            cout << "Invalid choice! Please try again.\n";
            break;
        }

        if (choice != 5) {
            cout << "\nPress Enter to return to the main menu...";
            cin.ignore();
            cin.get();
        }
    } while (choice != 5);
}


void eventRegistration(Account& acc, vector<Event>& events) {
    eventInput(acc, events);
}

int main() {
    accountManagement();
    return 0;
}