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

void createActivity(const Event& event);
void editActivity(const string& filename, const Event& event);
void viewActivity(const string& filename, const Event& event);
void deleteActivity(const string& filename, const Event& event);
void loopMenu(const vector<string>& menu, int* selection = nullptr, string title = "", bool runInput = false);


// ==== Validation =====
void checkInt(int &i) {
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
		cout << "Enter Customer IC (e.g., 990101-01-1234): ";
		cin >> ic;

		// Remove any '-' characters
		for (size_t i = 0; i < ic.length(); i++) {
			if (ic[i] == '-') {
				ic.erase(i, 1);
				i--;
			}
		}

		// Check if all characters are digits
		valid = true;
		if (ic.length() != 12) valid = false;
		else {
			for (size_t i = 0; i < ic.length(); i++) {
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
		for (size_t i = 0; i < hpNo.length(); i++) {
			if (hpNo[i] == '-' || hpNo[i] == ' ') {
				hpNo.erase(i, 1);
				i--;
			}
		}

		// Check if all characters are digits
		valid = true;
		if (hpNo.length() < 10 || hpNo.length() > 11) valid = false;
		else {
			for (size_t i = 0; i < hpNo.length(); i++) {
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

// ==== Loop ====
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
void vectorLoopAndselectionInput(int& selection, int typeOutput = 0, string title = "", vector<Event>* records = nullptr, bool* run = nullptr) {
	int min = 0;
	size_t max = 0;

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
void loadEvents(vector<string>* lines = nullptr, const string& filename = "assignment.txt") {
	ifstream file(filename);
	if (!file.is_open()) return;

	string line;
	if (filename == "assignment.txt") {
		events.clear();

		while (getline(file, line)) {
			stringstream ss(line);
			Event e;
			string paidStr, priceStr, addOnPriceStr, totalPrice, totalGuest, basePrice;

			getline(ss, e.customer.customerIC, ',');
			getline(ss, e.customer.customerName, ',');
			getline(ss, e.customer.customerHpNo, ',');

			string ageStr;
			getline(ss, ageStr, ',');
			e.deceased.age = ageStr.empty() ? 0 : stoi(ageStr);

			// Deceased death date
			string y, m, d;
			getline(ss, y, ','); e.deceased.deadDay.year = stoi(y);
			getline(ss, m, ','); e.deceased.deadDay.month = stoi(m);
			getline(ss, d, ','); e.deceased.deadDay.date = stoi(d);

			// Event date
			getline(ss, y, ','); e.date.year = std::stoi(y);
			getline(ss, m, ','); e.date.month = std::stoi(m);
			getline(ss, d, ','); e.date.date = std::stoi(d);

			// Package
			getline(ss, e.package.name, ',');
			getline(ss, priceStr, ',');
			e.package.price = priceStr.empty() ? 0.0 : stod(priceStr);

			// AddOn
			getline(ss, e.addOn.name, ',');
			getline(ss, addOnPriceStr, ',');
			e.addOn.price = addOnPriceStr.empty() ? 0.0 : stod(addOnPriceStr);

			getline(ss, totalGuest, ',');
			e.totalGuest = totalGuest.empty() ? 0 : stoi(totalGuest);

			getline(ss, basePrice, ',');
			e.basePrice = basePrice.empty() ? 0.0 : stod(basePrice);

			getline(ss, totalPrice, ',');
			e.totalPrice = totalPrice.empty() ? 0.0 : stod(totalPrice);


			getline(ss, paidStr, ',');
			e.paid = (paidStr == "1");

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

void saveEvents() {
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


// ===== Registration =====
void addOn(Event& e) {
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

void eventInput() {
	Event e;

	cin.ignore();
	cout << "\n[Event Registration]\n";

	// Customer info
	cout << "Enter Customer IC:";
	e.customer.customerIC = inputIC();
	cin.ignore();
	cout << "Enter Customer Name: ";
	getline(cin, e.customer.customerName);
	e.customer.customerHpNo = inputHp();

	// Deceased info
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

	cout << "\nEvent successfully registered!\n";

	return;

}

void readEvent() {
	string IC = "";
	bool found = false;
	vector<string> dummy;

	loadEvents(&dummy, "assignment.txt");

	cout << "Please enter your IC: ";
	IC = inputIC();

	for (int i = 0; i < events.size(); i++) {
		if (events[i].customer.customerIC == IC) {
			cout << "Customer Name: " << events[i].customer.customerName << endl;
			cout << "Customer Phone Number: " << events[i].customer.customerHpNo << endl;
			cout << "Deceased Name: " << events[i].deceased.deceasedName << endl;
			cout << "Deceased Age: " << events[i].deceased.age << endl;
			cout << "Date: " << events[i].date.year << "-"
				<< events[i].date.month << "-"
				<< events[i].date.date << endl;
			cout << "Package: " << events[i].package.name
				<< " (" << events[i].package.detail << ")" << endl;
			cout << "Add-On: " << events[i].addOn.name
				<< " ($" << events[i].addOn.price << ")" << endl;
			cout << "Total Guests: " << events[i].totalGuest << endl;
			cout << "Base Price: $" << events[i].basePrice << endl;
			cout << "Total Price: $" << events[i].totalPrice << endl;
			cout << "Paid: " << (events[i].paid ? "Yes" : "No") << endl;
			cout << "-----------------------------" << endl;
		}
		else if (!found) {
			cout << "No event found with this IC.\n";

		}
	}
}

void updateEvent() {
	string IC;
	cout << "Enter your IC: ";
	IC = inputIC();
}

void eventRegistration() {
	vector<string> menu = { "Create Event", "Read Event", "Update Event","Exit"};
	int selection = 0;
	bool run = true;

	while (selection == 0) {
		system("cls");

		loopMenu(menu, &selection, "Monitor a Created Event", true);

		switch (selection) {
		case 1:
			eventInput(); // Register new event
			break;
		case 2:
			readEvent(); // Read existing events
			break;
		case 3:
			updateEvent();
			break;
		case 4:
			cout << "Exiting...";
			break;
		default:
			cout << "Invalid input: Please enter integer between 1 - 3.";
		}
	}
}


// ===== Payment =====
void eventPayment() {
	cout << "\n[Event Payment]\n";
	cout << "Select an event to pay for:\n";

	for (size_t i = 0; i < events.size(); i++) {
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
		saveEvents();
		cout << "Payment completed.\n";
	}
}


// ===== Monitoring =====
void eventMonitoring() {
	vector<Event> paidEvents;
	vector<string> menu = { "Create Activity","View Activity","Edit Activity","Delete Activity" };
	int userSelection;
	int selection = 0;
	bool run = true;

	while (selection == 0) {
		system("cls");
		// call function to output the paid record and let user select 
		vectorLoopAndselectionInput(userSelection, 1, "[Event Monitoring]\n", &paidEvents, &run);

		if (run) {
			cout << endl;
			system("cls");
			loopMenu(menu, &selection, "Monitor a Created Event", true); // select on specific user

			switch (selection) {
			case 1:
				createActivity(paidEvents.at(userSelection - 1));
				createActivity(paidEvents.at(userSelection - 1));
				break;
			case 2:
				viewActivity("activity.txt", paidEvents.at(userSelection - 1));
				break;
			case 3:
				editActivity("activity.txt", paidEvents.at(userSelection - 1));
				break;
			case 4:
				deleteActivity("activity.txt", paidEvents.at(userSelection - 1));
				break;
			}
		}
		else break;
	}
}

void createActivity(const Event& event) {
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
	//loadEvents(&lines, filename);
}

void viewActivity(const string& filename, const Event& event) {
	system("cls");
	cout << "=====================================\n";
	cout << "   " << "View Activity" << "   \n";
	cout << "=====================================\n";

	vector<string> lines;
	loadEvents(&lines, filename);

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
			cout << " Activity for " << customerName << " - " << deceasedName << "\n";
			cout << "=====================================\n";

			if (tokens.size() >= 10) {
				cout << "Initiator        : " << tokens[3] << endl;
				cout << "Invitees         : " << tokens[4] << endl;
				cout << "Type of Activity : " << tokens[5] << endl;
				cout << "Amount           : " << tokens[6] << endl;
				cout << "Description      : " << tokens[7] << endl;
				cout << "Activity Date    : " << tokens[8] << endl;
				cout << "Activity Time    : " << tokens[9] << endl;
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
	loadEvents(&lines, filename);
	loadEvents(&lines, filename);

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
			matched.push_back((int)i);
		}
	}

	if (matched.empty()) {
		cout << "No matching activities for this event in " << filename << endl;
		return -1;
	}

	cout << "\nMatching activities (" << matched.size() << "):\n";
	for (size_t j = 0; j < matched.size(); ++j) {
		cout << j + 1 << ") " << lines[matched[j]] << '\n';
	}

	cout << "Select an activity (1-" << matched.size() << "): ";
	int choice;
	if (!(cin >> choice) || choice < 1 || choice >(int)matched.size()) {
		cout << "Invalid choice.\n";
		cin.clear();
		cin.ignore();
		return -1;
	}
	cin.ignore();
	cin.ignore();

	return matched[choice - 1];
}

void editActivity(const string& filename, const Event& event) {
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

	if (tokens.size() < 10) {
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


int main() {

	int choice = 0;
	loadEvents(); // Load from file when program starts

	vector<string> menu = { "Register Funeral Event", "Payment for an Registered Event", "Monitor a Created Event", "Exit" };
	do {
		system("cls"); // Use "clear" for Mac/Linux

		loopMenu(menu, nullptr, "Funeral Event Management System");

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