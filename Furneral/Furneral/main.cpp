#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <format>
#include <chrono>
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

vector<Event> events;

void registerAcc();
Account loginAcc(vector<Event>& events);
void accountManagement(vector<Event>& events);
void afterLogin(Account& acc, vector<Event>& events);

void updateEvent(vector<Event>& events);
void eventInput(Account& acc, vector<Event>& events);
void eventRegistration(Account& acc, vector<Event>& events);
void delEvent(Account& acc, vector<Event>& events);
void readEvent(Account& acc, vector<Event>& events);

void createActivity(const Event& event, const Account& acc);
void editActivity(const string& filename, const Event& event, const Account& acc);
void viewActivity(const string& filename, const Event& event, const Account& acc);
void deleteActivity(const string& filename, const Event& event, const Account& acc);
void loopMenu(const vector<string>& menu, int* selection = nullptr, string title = "", bool runInput = false);

//Validation
void checkInt(int& i);
string inputIC();
string inputHp();
bool isDeathDateValid(int y, unsigned m, unsigned d);
void inputDeathDate(Event& e);
vector<sys_days> generateFuneralDates();
void inputFuneralDate(Event& e);

// ==== Validation =====
void checkInt(int& i) {
	while (true) {
		if (cin.fail()) {
			cin.clear();
			cin.ignore();
			cout << "Invalid input! Please enter an integer: ";
			cin >> i;
		}
		else {
			break;
		}
	};
}

string inputIC() {
	string ic;
	bool valid;

	while (true) {
		cout << "Enter IC (e.g., 990101-01-1234): ";
		cin >> ic;

		// Remove any '-' characters
		for (int i = 0; i < ic.length(); i++) {
			if (ic[i] == '-') {
				ic.erase(i, 1);
				i--;
			}
		}

		// Check if all characters are digits
		valid = true;
		if (ic.length() != 12) valid = false;
		else {
			for (int i = 0; i < ic.length(); i++) {
				if (!isdigit(ic[i])) {
					valid = false;
					break;
				}
			}
		}

		if (!valid) {
			cout << "Invalid IC. Please enter exactly 12 digits.\n";
		}
		else {
			return ic;
		}
	}
}

string inputHp() {
	bool valid;
	string hpNo;

	while (true) {
		cout << "Enter Customer Phone Number (e.g., 011-2727 5569): ";
		getline(cin, hpNo);

		// Remove any '-' characters
		for (int i = 0; i < hpNo.length(); i++) {
			if (hpNo[i] == '-' || hpNo[i] == ' ') {
				hpNo.erase(i, 1);
				i--;
			}
		}

		// Check if all characters are digits
		valid = true;
		if (hpNo.length() < 10 || hpNo.length() > 11) valid = false;
		else {
			for (int i = 0; i < hpNo.length(); i++) {
				if (!isdigit(hpNo[i])) {
					valid = false;
					break;
				}
			}
		}

		if (!valid) {
			cout << "Invalid phone number. Please enter in total 9 - 10 digits.\n";
		}
		else {
			return hpNo;
		}
	}

}

//date validation
bool isDeathDateValid(int y, unsigned m, unsigned d) {
	year_month_day ymd{ year{y}, month{m}, day{d} };
	if (!ymd.ok()) return false;

	sys_days today = floor<days>(system_clock::now());

	if (sys_days{ ymd } > today) return false;
	if (y < 1900) return false;
	return true;
}

void inputDeathDate(Event& e) {
	int y, m, d;
	cout << "Enter Date of Death (YYYY MM DD): ";
	cin >> y >> m >> d;
	while (!isDeathDateValid(y, m, d)) {
		cout << "Invalid date! Must be 1900 - today. Try again: ";
		cin >> y >> m >> d;
	}
	e.deceased.deadDay.year = y;
	e.deceased.deadDay.month = m;
	e.deceased.deadDay.date = d;
}

vector<sys_days> generateFuneralDates() {
	sys_days today = floor<days>(system_clock::now());
	sys_days start = today + days{ 7 };   // earliest funeral date
	sys_days end = today + days{ 14 };  // latest funeral date

	vector<sys_days> dates;
	for (sys_days d = start; d <= end; d += days{ 1 })
		dates.push_back(d);

	return dates;
}

void inputFuneralDate(Event& e) {
	vector<sys_days> dates = generateFuneralDates();
	cout << "\nAvailable Funeral Dates (1 - 2 weeks from today):\n";
	for (int i = 0; i < dates.size(); i++) {
		year_month_day ymd = dates[i];
		cout << i + 1 << ". "
			<< int(ymd.year()) << "-"
			<< unsigned(ymd.month()) << "-"
			<< unsigned(ymd.day()) << "\n";
	}

	int choice;
	cout << "Choose a date (1 - " << dates.size() << "): ";
	cin >> choice;
	while (choice < 1 || choice > dates.size()) {
		cout << "Invalid choice! Try again: ";
		cin >> choice;
	}

	year_month_day selected = dates[choice - 1];
	e.date.year = int(selected.year());
	e.date.month = unsigned(selected.month());
	e.date.date = unsigned(selected.day());
}


vector<Event> getUnpaidEventsByIC(string IC, vector<Event>& events) {
	vector<Event> temp;
	for (int i = 0; i < events.size(); i++) {
		if (events[i].customer.customerIC == IC && !events[i].paid) {
			temp.push_back(events[i]);
			events.erase(events.begin() + i);
			i--;
		}
	}
	return temp;
}

// ==== Loop ====
// output vector (1 for paid, 2 for unpaid , else all)  // record for record the specific type record
void vectorLoop(const vector<Event>& events, int typeOutput = 0, string title = "", vector<Event>* records = nullptr) {
	int count = 0;
	if (records) records->clear();

	if (title != "") cout << title << endl;

	for (int i = 0; i < events.size(); i++) {
		// if only want to output the paid record 
		switch (typeOutput) {
			// output the paid record only 
		case 1:
			if (events[i].paid) {
				cout << ++count << ". " << events[i].deceased.deceasedName
					<< " (Customer: " << events[i].customer.customerName << ") - Package: "
					<< events[i].package.name << " - Status: "
					<< "PAID" << "\n";
				if (records) records->push_back(events.at(i));
			}
			break;
			// output the unpaid record only 
		case 2:
			if (!events[i].paid) {
				cout << ++count << ". " << events[i].deceased.deceasedName
					<< " (Customer: " << events[i].customer.customerName << ") - Package: "
					<< events[i].package.name << " - Status: "
					<< "UNPAID" << "\n";
				if (records) records->push_back(events.at(i));
			}
			break;
			// output all record
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

		cout << format("Please enter between {} - {} (0 for exit): ", min, max);
		cin >> selection;

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

// loop a menu
void loopMenu(const vector<string>& menu, int* selection, const string title, bool runInput) {
	if (title != "") {
		cout << "=====================================\n";
		cout << "   " << title << "   \n";
		cout << "=====================================\n";
	}
	int i = 0;
	for (const auto& m : menu) {
		cout << ++i << ". " << m << endl;
	}
	if (runInput) selctionCheckInput(1, int(menu.size()), *selection, &menu, &title);
}


// ===== File handling ===== (allow to read paid, unpaid or all record)
void loadEvents(vector<Event>& events, vector<string>* lines = nullptr, const string& filename = "assignment.txt") {
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

			// Deceased death date
			getline(ss, s, ','); e.deceased.deadDay.year = stoi(s);
			getline(ss, s, ','); e.deceased.deadDay.month = stoi(s);
			getline(ss, s, ','); e.deceased.deadDay.date = stoi(s);

			// Event date
			getline(ss, s, ','); e.date.year = std::stoi(s);
			getline(ss, s, ','); e.date.month = std::stoi(s);
			getline(ss, s, ','); e.date.date = std::stoi(s);

			// Package
			getline(ss, e.package.name, ',');
			getline(ss, s, ',');
			e.package.price = s.empty() ? 0.0 : stod(s);

			// AddOn
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
		if (lines) {
			lines->clear();
			while (getline(file, line)) {
				lines->push_back(line);
			}
		}
	}

	file.close();
}

void saveEvents(const vector<Event>& events) {
	ofstream file("assignment.txt");
	for (auto& e : events) {
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

	for (auto& line : lines) {
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

// ===== Account =====
void registerAcc() {
	string ic, name, password, hp;
	string line;

	system("cls");
	cout << "\n[Register Account]\n";
	ic = inputIC();
	cout << "Enter your name: ";
	cin.ignore();
	getline(cin, name);
	hp = inputHp();
	cout << "Enter your password: ";
	getline(cin, password);
	ifstream readFile("accounts.txt");

	ofstream file("accounts.txt", ios::app);
	if (file.is_open()) {
		while (getline(readFile, line)) {
			stringstream ss(line);
			string tempIc, tempName, tempPass, tempHp;
			getline(ss, tempIc, ',');
			getline(ss,tempName, ',');
			getline(ss, tempPass, ',');
			getline(ss, tempHp, ',');
			if (tempIc == ic) {
				cout << "Error: IC already registered.\n";
				file.close();
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
}

Account loginAcc(vector<Event>& events) {
	Account acc;
	string icInput, passwordInput;

	system("cls");
	cout << "\n[Login Account]\n";
	icInput = inputIC();
	cout << "Enter your password: ";
	cin >> passwordInput;

	ifstream file("accounts.txt");
	if (!file.is_open()) {
		cout << "Error: Unable to open accounts.txt\n";
		return acc;
	}

	string line;
	bool found = false;
	while (getline(file, line)) {
		if (line.empty()) continue;
		stringstream ss(line);
		string ic, name, pass, hp;
		getline(ss, ic, ',');
		getline(ss, name, ',');
		getline(ss, pass, ',');
		getline(ss, hp, ',');

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
		afterLogin(acc, events);
	}
	else {
		cout << "Login failed: Incorrect IC or password.\n";
	}

	return acc;
}


void accountManagement(vector<Event>& events) {
	vector<string> menu = { "Register Account", "Login Account", "Exit" };
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
			loginAcc(events);
			break;
		case 3:
			cout << "Exiting...\n";
			run = false;
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
		cout << "------------------------------\n";

		for (int i = 0; i < addons.size(); i++) {
			cout << i + 1 << ". " << addons[i].name << " - RM " << addons[i].price << endl;
		}
		cout << "0. No Add-on\n";

		int choice;
		cout << "Enter choice: ";
		cin >> choice;

		if (cin.fail()) {
			cin.clear(); // clear error flag
			cout << "Invalid input! Please enter a number.\n";
			continue;
		}

		if (choice == 0) {
			e.addOn = { "No Add-on", 0.0 };
			break;
		}
		else if (choice >= 1 && choice <= addons.size()) {
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
	cout << "\n[Select Event]\n";
	e.basePrice = e.totalGuest * 30;

	vector<Package> packages = {
		{"Basic Funeral Package", "Professional service fees ...", 3800.00},
		{"Standard Funeral Package", "Includes Basic + embalming ...", 5500.00},
		{"Premium Funeral Package", "Includes Standard + premium casket ...", 9500.00}
	};

	for (int i = 0; i < packages.size(); i++) {
		cout << i + 1 << ". " << packages[i].name << endl
			<< setw(2) << "Detail: " << packages[i].detail << endl
			<< setw(2) << "RM" << packages[i].price << endl;
	}

	int choice;
	cout << "Choose package (1-" << packages.size() << "): ";
	cin >> choice;
	e.package = packages.at(choice - 1);

	cin.ignore();
	string addOnChoice = "";
	cout << "Do you want to add on (Yes or No): ";
	getline(cin, addOnChoice);

	if (addOnChoice == "Yes") {
		addOn(e);
	}
	else {
		e.addOn = { "No Add-on", 0.0 };
	}

	e.totalPrice = e.package.price + e.addOn.price + e.basePrice;
}

void eventInput(Account& acc, vector<Event>& events) {
	system("cls");
	Event e;

	cin.ignore();
	cout << "\n[Event Registration]\n";

	// Customer info
	e.customer.customerIC = acc.customer.customerIC;
	e.customer.customerName = acc.customer.customerName;
	e.customer.customerHpNo = acc.customer.customerHpNo;

	cout << "Enter Deceased's Name: ";
	getline(cin, e.deceased.deceasedName);

	cout << "Enter Deceased's Age: ";
	cin >> e.deceased.age;
	checkInt(e.deceased.age);
	cin.ignore();

	inputDeathDate(e);
	inputFuneralDate(e);

	cin.ignore();

	cout << "Enter total guests: ";
	cin >> e.totalGuest;
	checkInt(e.totalGuest);

	selectPackage(e);

	e.paid = false;

	events.push_back(e);
	saveEvents(events); 
	cout << "\nEvent successfully registered!\n";
}


void readEvent(Account& acc, vector<Event>& events) {
	system("cls");
	string IC = "";
	bool found = false;
	vector<string> dummy;

	loadEvents(events, &dummy, "assignment.txt");

	int count = 0;
	for (int i = 0; i < events.size(); i++) {
		if (events[i].customer.customerIC == acc.customer.customerIC) {
			count++;
			cout << "No " << count << ": " << endl;
			cout << "  Deceased Name  :" << events[i].deceased.deceasedName << endl;
			cout << "  Deceased Age   :" << events[i].deceased.age << endl;
			cout << "  Date           :" << events[i].date.year << "-"
				<< events[i].date.month << "-"
				<< events[i].date.date << endl;
			cout << "  Package        :" << events[i].package.name << endl;
			cout << "  Add-On         :" << events[i].addOn.name
				<< "   ($" << events[i].addOn.price << ")" << endl;
			cout << "  Total Guests   :" << events[i].totalGuest << endl;
			cout << "  Base Price     : RM" << events[i].basePrice << endl;
			cout << "  Total Price    : RM" << events[i].totalPrice << endl;
			cout << "  Paid           :" << (events[i].paid ? "Yes" : "No") << endl;
			cout << "-----------------------------" << endl;
		}
		else if (!found) {
			cout << "No event found with this IC.\n";
		}
	}
}

void updateEvent(vector<Event>& events) {
	system("cls");
	loadEvents(events);

	string IC = inputIC();

	// Collect all unpaid events for this IC
	vector<Event> userEvents;
	for (int i = 0; i < events.size(); i++) {
		if (events[i].customer.customerIC == IC && !events[i].paid) {
			userEvents.push_back(events[i]);
		}
	}

	if (userEvents.empty()) {
		cout << "No unpaid events found with this IC.\n";
		return;
	}

	// Display unpaid events using vectorLoop
	cout << "\n[Your Unpaid Registered Events]\n";
	vectorLoop(events, 2, "", &userEvents); // 2 = unpaid, but we only added unpaid to userEvents

	// Let user select event to update
	int choice;
	cout << "\nChoose event to update (1-" << userEvents.size() << "): ";
	cin >> choice;
	cin.ignore();

	if (choice < 1 || choice > userEvents.size()) {
		cout << "Invalid choice.\n";
		return;
	}

	Event& e = userEvents[choice - 1];

	bool editing = true;
	while (editing) {
		system("cls");
		cout << "\n[Update Event]\n";
		cout << "1. Funeral Date (" << e.date.year << "-"
			<< e.date.month << "-" << e.date.date << ")\n";
		cout << "2. Guests (" << e.totalGuest << ")\n";
		cout << "3. Package (" << e.package.name << ")\n";
		cout << "4. Add-On (" << e.addOn.name << ")\n";
		cout << "0. Finish Editing\n";
		cout << "Choose field to update: ";
		int opt;
		cin >> opt;
		cin.ignore();

		switch (opt) {
		case 1: inputFuneralDate(e); break;
		case 2:
			cout << "Enter new total guests: ";
			cin >> e.totalGuest;
			checkInt(e.totalGuest);
			cin.ignore();
			break;
		case 3: selectPackage(e); break;
		case 4: addOn(e); break;
		case 0: editing = false; break;
		default: cout << "Invalid choice.\n"; break;
		}

		// Recalculate total price
		e.totalPrice = e.package.price + e.addOn.price + (e.totalGuest * 30);
	}

	//Update events vector: remove old unpaid events for this IC
	for (int i = 0; i < events.size(); i++) {
		if (events[i].customer.customerIC == IC && !events[i].paid) {
			events.erase(events.begin() + i);
			i--; // adjust index
		}
	}

	// Insert the updated events back
	events.insert(events.end(), userEvents.begin(), userEvents.end());
	saveEvents(events); 
	cout << "Event updated successfully!\n";
}


void delEvent(Account& acc, vector<Event>& events) {
	system("cls");
	cout << "\n[Delete Event]\n";
	loadEvents(events);

	vector<Event> temp = getUnpaidEventsByIC(acc.customer.customerIC, events);

	if (temp.empty()) {
		cout << "No unpaid events found with this IC.\n";
		return;
	}

	// Display events
	cout << "\n[Your Registered Events]\n";
	for (int j = 0; j < temp.size(); ++j) {
		cout << j + 1 << ". "
			<< temp[j].deceased.deceasedName
			<< " | Date: " << temp[j].date.year << "-"
			<< temp[j].date.month << "-"
			<< temp[j].date.date
			<< " | Package: " << temp[j].package.name << "\n";
	}

	// Get user choice
	cout << "\nChoose event to delete: ";
	int choice;
	cin >> choice;
	cin.ignore();

	if (choice < 1 || choice > temp.size()) {
		cout << "Invalid choice.\n";
		events.insert(events.end(), temp.begin(), temp.end());
		return;
	}

	temp.erase(temp.begin() + (choice - 1));
	events.insert(events.end(), temp.begin(), temp.end());
	saveEvents(events);  
	cout << "Event deleted successfully.\n";
}


void eventRegistration(Account& acc, vector<Event>& events) {
	vector<string> menu = { "Create Event", "Read Event", "Update Event","Delete Event", "Exit" };
	int selection = 0;
	bool run = true;

	while (selection == 0) {
		system("cls");
		loopMenu(menu, &selection, "Event Registration", true);
		if (run) {
			switch (selection) {
			case 1:
				eventInput(acc, events);
				break;
			case 2:
				readEvent(acc, events);  
				break;
			case 3:
				updateEvent(events);  
				break;
			case 4:
				delEvent(acc, events); 
				break;
			case 5:
				cout << "Exiting...";
				break;
			default:
				cout << "Invalid input: Please enter integer between 1 - 4.";
				break;
			}
		}
		else break;
	}
}


void eventPayment(vector<Event>& events) {
	cout << "\n[Event Payment]\n";
	cout << "Select an event to pay for:\n";

	for (int i = 0; i < events.size(); i++) {
		if (!events[i].paid)
			cout << i + 1 << ". Deceased Name:  " << events[i].deceased.deceasedName
			<< " (Register Customer: " << events[i].customer.customerName << ")";

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
		saveEvents(events);
		cout << "Payment completed.\n";
	}
}


void eventMonitoring(const Account& acc, vector<Event>& events) {
	loadEvents(events);

	vector<Event> sPaidEvents;
	for (const Event& e : events) {
		if (e.customer.customerIC == acc.customer.customerIC && e.paid) {
			sPaidEvents.push_back(e);
		}
	}

	if (sPaidEvents.empty()) {
		cout << "No paid events found for your account.\n";
		system("pause");
		return;
	}

	vector<string> menu = { "Create Activity", "View Activity", "Edit Activity", "Delete Activity", "Back" };
	int userSelection = 0;
	int selection = 0;
	bool run = true;

	while (run) {
		system("cls");

		cout << "[Event Monitoring - Your Paid Events]\n";
		cout << "=====================================\n";
		for (int i = 0; i < sPaidEvents.size(); i++) {
			cout << i + 1 << ". " << sPaidEvents[i].deceased.deceasedName
				<< " (Date: " << sPaidEvents[i].date.year << "-"
				<< sPaidEvents[i].date.month << "-"
				<< sPaidEvents[i].date.date << ")\n";
		}
		cout << "0. Back to Main Menu\n";

		cout << "Select an event: ";
		cin >> userSelection;

		if (userSelection == 0) {
			run = false;
			continue;
		}

		if (userSelection < 1 || userSelection > sPaidEvents.size()) {
			cout << "Invalid selection. Please try again.\n";
			system("pause");
			continue;
		}

		Event selectedEvent = sPaidEvents[userSelection - 1];

		selection = 0;
		system("cls");
		loopMenu(menu, &selection, "Activity Management for " + selectedEvent.deceased.deceasedName, true);

		switch (selection) {
		case 1:
			createActivity(selectedEvent, acc);
			break;
		case 2:
			viewActivity("activity.txt", selectedEvent, acc);
			break;
		case 3:
			editActivity("activity.txt", selectedEvent, acc);
			break;
		case 4:
			deleteActivity("activity.txt", selectedEvent, acc);
			break;
		case 5:
			break;
		default:
			cout << "Invalid selection.\n";
			break;
		}

		if (selection != 5) {
			system("pause");
		}
	}
}


void createActivity(const Event& event, const Account& acc) {
	system("cls");
	vector<string> lines;
	string filename = "activity.txt";
	Activity activity;

	cout << "=====================================\n";
	cout << "   " << "Create Activity" << "   \n";
	cout << "=====================================\n";

	cin.ignore();
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

	cout << "Date (dd mm yy): ";
	cin >> activity.date.date >> activity.date.month >> activity.date.year;

	cout << "Date (hh mm): ";
	cin >> activity.time.hours >> activity.time.minute;

	stringstream ss;
	ss << acc.customer.customerIC << ","
		<< event.customer.customerName << ","
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
}

void viewActivity(const string& filename, const Event& event, const Account& acc) {
	system("cls");
	cout << "=====================================\n";
	cout << "   " << "View Activity" << "   \n";
	cout << "=====================================\n";

	vector<string> lines;
	vector<Event> dummyEvents; 
	loadEvents(dummyEvents, &lines, filename); 

	bool found = false;

	for (const auto& line : lines) {
		if (line.empty()) continue;

		stringstream ss(line);
		string token;
		vector<string> tokens;

		while (getline(ss, token, ',')) {
			tokens.push_back(token);
		}

		if (tokens.size() < 3) continue;

		string customerIc = tokens[0];
		string customerName = tokens[1];
		string deceasedName = tokens[2];
		string eventDate = tokens[3];

		string targetDate = to_string(event.date.date) + " " +
			to_string(event.date.month) + " " +
			to_string(event.date.year);

		if (customerIc == acc.customer.customerIC &&
			customerName == event.customer.customerName &&
			deceasedName == event.deceased.deceasedName &&
			eventDate == targetDate)
		{
			found = true;
			cout << "=====================================\n";
			cout << " Activity for " << customerName << " - " << deceasedName << "\n";
			cout << "=====================================\n";

			if (tokens.size() >= 11) {
				cout << "Initiator        : " << tokens[4] << endl;
				cout << "Invitees         : " << tokens[5] << endl;
				cout << "Type of Activity : " << tokens[6] << endl;
				cout << "Amount           : " << tokens[7] << endl;
				cout << "Description      : " << tokens[8] << endl;
				cout << "Activity Date    : " << tokens[9] << endl;
				cout << "Activity Time    : " << tokens[10] << endl;
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
	vector<Event> dummyEvents; 
	loadEvents(dummyEvents, &lines, filename); 

	vector<int> matched;
	const string targetDate =
		to_string(event.date.date) + " " +
		to_string(event.date.month) + " " +
		to_string(event.date.year);

	for (int i = 0; i < lines.size(); ++i) {
		if (lines[i].empty()) continue;

		stringstream ss(lines[i]);
		string customerIc, customerName, deceasedName, eventDate;
		if (!getline(ss, customerIc, ',')) continue;
		if (!getline(ss, customerName, ',')) continue;
		if (!getline(ss, deceasedName, ',')) continue;
		if (!getline(ss, eventDate, ',')) continue;

		if (customerName == event.customer.customerName &&
			deceasedName == event.deceased.deceasedName &&
			eventDate == targetDate) {
			matched.push_back((int)i);
		}
	}

	if (matched.empty()) {
		cout << "No matching activities for this event in " << filename << endl;
		return -1;
	}

	cout << "\nMatching activities (" << matched.size() << "):\n";
	for (int j = 0; j < matched.size(); ++j) {
		cout << j + 1 << ") " << lines[matched[j]] << '\n';
	}

	cout << "Select an activity (1-" << matched.size() << "): ";
	int choice;
	if (!(cin >> choice) || choice < 1 || choice >(int)matched.size()) {
		cout << "Invalid choice.\n";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return -1;
	}
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	return matched[choice - 1];
}

void editActivity(const string& filename, const Event& event, const Account& acc) {
	system("cls");
	cout << "=====================================\n";
	cout << "   " << "Edit Activity" << "   \n";
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

	if (tokens.size() < 11) {
		cout << "Malformed record, cannot edit: " << lines[idx] << '\n';
		return;
	}

	auto editField = [&](int fieldIndex, const string& label) {
		cout << label << " [" << tokens[fieldIndex] << "]: ";
		string in;
		getline(cin, in);
		if (!in.empty()) tokens[fieldIndex] = in;
		};

	cout << "\n=== Edit Activity Fields (press Enter to keep original) ===\n";
	editField(4, "Initiator");
	editField(5, "Invitees");
	editField(6, "Type");
	editField(7, "Amount");
	editField(8, "Description");
	editField(9, "Activity Date (dd mm yy)");
	editField(10, "Activity Time (hh mm)");

	stringstream os;
	for (int i = 0; i < tokens.size(); ++i) {
		if (i) os << ',';
		os << tokens[i];
	}
	lines[idx] = os.str();

	overwriteFile(filename, lines);
	cout << "Activity updated.\n";
}

void deleteActivity(const string& filename, const Event& event, const Account &acc) {
	system("cls");
	cout << "=====================================\n";
	cout << "   " << "Delete Activity" << "   \n";
	cout << "=====================================\n";

	vector<string> lines;
	int idx = selectActivityIndex(filename, event, lines);
	if (idx < 0) return;

	cout << "Delete this record? \n" << lines[idx] << "\nConfirm (y/n): ";
	char yn;
	cin >> yn;
	cin.ignore();
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
	loadEvents(events); 

	vector<string> menu = { "Register Funeral Event", "Payment for an Registered Event", "Monitor a Created Event", "Exit" };
	do {
		system("cls");

		loopMenu(menu, nullptr, "Funeral Event Management System");

		cout << "-------------------------------------\n";
		cout << "Enter your choice: ";
		cin >> choice;

		switch (choice) {
		case 1:
			eventRegistration(acc, events);  
			break;
		case 2:
			if (events.empty()) cout << "\n[ERROR] No events registered yet.\n";
			else eventPayment(events);  
			break;
		case 3:
			if (events.empty()) cout << "\n[ERROR] No events registered yet.\n";
			else eventMonitoring(acc, events); 
			break;
		case 4:
			saveEvents(events);  
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
}


int main() {
	vector<Event> events;
	accountManagement(events); 
	return 0;
}