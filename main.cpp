#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

struct Appliance {
    string name;
    double powerW;      // watts
    double hoursPerDay; // 0 - 24

    double energyKWhPerDay() const {
        return (powerW * hoursPerDay) / 1000.0;
    }
};

static void clearBadInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}


void searchAppliance(const vector<Appliance>& appliances) {
    string searchName;
    cout << "\nEnter the name of the appliance to search for: ";
    cin.ignore(); // Clears the input buffer
    getline(cin, searchName);

    bool found = false;
    for (const auto& item : appliances) {
        if (item.name == searchName) {
            cout << "Appliance Found!" << endl;
            cout << "Name: " << item.name << " | Power: " << item.powerW << "W" << endl;
            found = true;
            break; 
        }
    }

    if (!found) {
        cout << "Appliance not found in the database." << endl;
    }
}

static string toLowerStr(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

int menu() {
    cout << "\n==============================\n";
    cout << "     Electrical Load Monitoring\n";
    cout << "==============================\n";
    cout << "1. Register appliance\n";
    cout << "2. View all appliances\n";
    cout << "3. Search appliance by name\n";
    cout << "4. Energy summary (kWh/day)\n";
    cout << "5. Billing summary\n";
    cout << "0. Exit\n";
    cout << "Choose: ";

    int choice;
    cin >> choice;

    if (cin.fail()) {
        clearBadInput();
        return -1;
    }
    return choice;
}

Appliance registerAppliance() {
    Appliance a{};

    // clear newline from previous cin >> ...
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // name validation
    do {
        cout << "Enter appliance name: ";
        getline(cin, a.name);
        if (a.name.empty()) cout << "Name must not be empty.\n";
    } while (a.name.empty());

    // power validation
    while (true) {
        cout << "Enter power rating (W): ";
        cin >> a.powerW;

        if (!cin.fail() && a.powerW > 0) break;

        cout << "Power must be a number greater than zero.\n";
        clearBadInput();
    }

    // hours validation
    while (true) {
        cout << "Enter usage hours per day (0 - 24): ";
        cin >> a.hoursPerDay;

        if (!cin.fail() && a.hoursPerDay >= 0 && a.hoursPerDay <= 24) break;

        cout << "Hours must be a number between 0 and 24.\n";
        clearBadInput();
    }

    cout << "Appliance registered successfully!\n";
    return a;
}

void viewAllAppliances(const vector<Appliance>& appliances) {
    if (appliances.empty()) {
        cout << "No appliances registered.\n";
        return;
    }

    cout << "\n================= APPLIANCES =================\n";
    cout << left
         << setw(5)  << "No."
         << setw(20) << "Name"
         << setw(12) << "Power(W)"
         << setw(12) << "Hours/day"
         << "\n----------------------------------------------\n";

    for (size_t i = 0; i < appliances.size(); i++) {
        const auto& a = appliances[i];
        cout << left
             << setw(5)  << (i + 1)
             << setw(20) << a.name
             << setw(12) << fixed << setprecision(2) << a.powerW
             << setw(12) << fixed << setprecision(2) << a.hoursPerDay
             << "\n";
    }
    cout << "================================================\n";
}

void searchApplianceByName(const vector<Appliance>& appliances) {
    if (appliances.empty()) {
        cout << "No appliances registered.\n";
        return;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string query;
    cout << "Enter appliance name to search: ";
    getline(cin, query);

    if (query.empty()) {
        cout << "Search term cannot be empty.\n";
        return;
    }

    string qLower = toLowerStr(query);
    bool found = false;

    for (const auto& a : appliances) {
        if (toLowerStr(a.name).find(qLower) != string::npos) {
            if (!found) {
                cout << "\nFound:\n";
                cout << left
                     << setw(20) << "Name"
                     << setw(12) << "Power(W)"
                     << setw(12) << "Hours/day"
                     << setw(12) << "kWh/day"
                     << "\n------------------------------------------------\n";
            }
            found = true;
            cout << left
                 << setw(20) << a.name
                 << setw(12) << fixed << setprecision(2) << a.powerW
                 << setw(12) << fixed << setprecision(2) << a.hoursPerDay
                 << setw(12) << fixed << setprecision(3) << a.energyKWhPerDay()
                 << "\n";
        }
    }

    if (!found) {
        cout << "No appliance matched: " << query << "\n";
    }
}

void showEnergySummary(const vector<Appliance>& appliances) {
    if (appliances.empty()) {
        cout << "No appliances registered.\n";
        return;
    }

    cout << "\n=============== ENERGY SUMMARY (per day) ===============\n";
    cout << left
         << setw(20) << "Name"
         << setw(12) << "Power(W)"
         << setw(12) << "Hours"
         << setw(12) << "kWh/day"
         << "\n--------------------------------------------------------\n";

    double total = 0.0;

    for (const auto& a : appliances) {
        double kwh = a.energyKWhPerDay();
        total += kwh;

        cout << left
             << setw(20) << a.name
             << setw(12) << fixed << setprecision(2) << a.powerW
             << setw(12) << fixed << setprecision(2) << a.hoursPerDay
             << setw(12) << fixed << setprecision(3) << kwh
             << "\n";
    }

    cout << "--------------------------------------------------------\n";
    cout << "TOTAL ENERGY: " << fixed << setprecision(3) << total << " kWh/day\n";
    cout << "========================================================\n";
}

void calculateBill(const vector<Appliance>& appliances) {
    if (appliances.empty()) {
        cout << "No appliances registered.\n";
        return;
    }

    double tariff;
    while (true) {
        cout << "Enter electricity tariff per kWh: ";
        cin >> tariff;

        if (!cin.fail() && tariff > 0) break;

        cout << "Tariff must be a positive number.\n";
        clearBadInput();
    }

    double totalEnergy = 0.0;
    for (const auto& a : appliances) {
        totalEnergy += a.energyKWhPerDay();
    }

    double totalCost = totalEnergy * tariff;

    cout << "\n===== BILL SUMMARY =====\n";
    cout << "Total Energy (per day): " << fixed << setprecision(3) << totalEnergy << " kWh/day\n";
    cout << "Tariff: " << fixed << setprecision(2) << tariff << " per kWh\n";
    cout << "Total Cost (per day): " << fixed << setprecision(2) << totalCost << "\n";
}

int main() {
    vector<Appliance> appliances;

    while (true) {
        int choice = menu();

        switch (choice) {
            case 1: {
                Appliance a = registerAppliance();
                appliances.push_back(a);
                break;
            }
            case 2:
                viewAllAppliances(appliances);
                break;

            case 3:
                searchApplianceByName(appliances);
                break;

            case 4:
                showEnergySummary(appliances);
                break;

            case 5:
                calculateBill(appliances);
                break;

            case 0:
                cout << "Goodbye!\n";
                return 0;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}