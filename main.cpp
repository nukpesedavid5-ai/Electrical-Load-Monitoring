#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <fstream>

using namespace std;

// --- STRUCTURES ---
struct Appliance {
    string name;
    double powerW;
    double hoursPerDay;

    double energyKWhPerDay() const {
        return (powerW * hoursPerDay) / 1000.0;
    }
};

// --- HELPER FUNCTIONS ---
static void clearBadInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static string toLowerStr(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

// Requirement 5.4: Save/Load Tariff so it persists
void saveTariff(double t) {
    ofstream fout("config.txt");
    if (fout.is_open()) {
        fout << t;
        fout.close();
    }
}

double loadTariff() {
    double t = 0.15; // Default if file missing
    ifstream fin("config.txt");
    if (fin.is_open()) {
        fin >> t;
        fin.close();
    }
    return t;
}

// --- CORE FUNCTIONALITIES ---
int menu() {
    cout << "\n====================================\n";
    cout << "   ELECTRICAL LOAD & BILLING SYSTEM\n";
    cout << "====================================\n";
    cout << "1. Register Appliance\n";
    cout << "2. View All Registered\n";
    cout << "3. Search Appliance by Name\n";
    cout << "4. Energy Summary (Total Load)\n";
    cout << "5. Generate Billing Report (File)\n";
    cout << "6. Delete Appliance\n";
    cout << "7. Update Electricity Tariff\n";
    cout << "8. View Usage Bar Chart\n";
    cout << "0. Exit System\n";
    cout << "------------------------------------\n";
    cout << "Choose Option: ";
    int choice;
    if (!(cin >> choice)) { clearBadInput(); return -1; }
    return choice;
}

void searchAppliance(const vector<Appliance>& apps) {
    if (apps.empty()) { cout << "No data available.\n"; return; }
    cout << "Enter name to search: ";
    string query;
    cin.ignore();
    getline(cin, query);
    
    string qLower = toLowerStr(query);
    bool found = false;
    for (const auto& a : apps) {
        if (toLowerStr(a.name).find(qLower) != string::npos) {
            cout << ">> Found: " << a.name << " [" << a.powerW << "W]\n";
            found = true;
        }
    }
    if (!found) cout << "no appliance found.\n";
}

int main() {
    vector<Appliance> appliances;
    double tariff = loadTariff(); 

    // Requirement 5.4: Load data on startup
    ifstream loadFile("appliance.txt");
    if (loadFile.is_open()) {
        string n; double p, h;
        while (loadFile >> n >> p >> h) {
            appliances.push_back({n, p, h});
        }
        loadFile.close();
    }

    while (true) {
        int choice = menu();
        if (choice == 0) break;

        switch (choice) {
            case 1: { // Register
                Appliance a;
                cout << "Name (no spaces): "; cin >> a.name;
                
                // Requirement 6: Validation
                while (true) {
                    cout << "Power (Watts > 0): ";
                    if (cin >> a.powerW && a.powerW > 0) break;
                    cout << "Error: Power must be positive Enter again.\n"; clearBadInput();
                }
                while (true) {
                    cout << "Daily Hours (0-24): ";
                    if (cin >> a.hoursPerDay && a.hoursPerDay >= 0 && a.hoursPerDay <= 24) break;
                    cout << "Error: Hours must be 0-24 Enter again.\n"; clearBadInput();
                }

                appliances.push_back(a);
                ofstream outFile("appliance.txt", ios::app);
                outFile << a.name << " " << a.powerW << " " << a.hoursPerDay << endl;
                cout << "Appliance logged successfully.\n";
                break;
            }
            case 2: { // View All
                if (appliances.empty()) cout << "Registry is empty.\n";
                else {
                    cout << left << setw(15) << "\nName" << setw(10) << "Watts" << "Hours/Day\n";
                    for(const auto &app : appliances) 
                        cout << left << setw(15) << app.name << setw(10) << app.powerW << app.hoursPerDay << endl;
                }
                break;
            }
            case 3: searchAppliance(appliances); break;

            case 4: { // Energy Summary
                double total = 0;
                cout << "\n--- DAILY LOAD PROFILE ---\n";
                for (const auto& app : appliances) {
                    double kwh = app.energyKWhPerDay();
                    total += kwh;
                    cout << "- " << app.name << ": " << fixed << setprecision(3) << kwh << " kWh\n";
                }
                cout << "--------------------------\n";
                cout << "TOTAL SYSTEM LOAD: " << total << " kWh/day\n";
                break;
            }
            case 5: { // Billing
                ofstream billFile("billing_summary.txt");
                double totalCost = 0;
                billFile << "=== OFFICIAL BILLING REPORT ===\n";
                billFile << "Tariff Rate: $" << tariff << "/kWh\n\n";
                for (const auto& app : appliances) {
                    double cost = app.energyKWhPerDay() * 30 * tariff;
                    totalCost += cost;
                    billFile << left << setw(15) << app.name << ": $" << fixed << setprecision(2) << cost << "\n";
                }
                billFile << "\nTOTAL ESTIMATED MONTHLY BILL: $" << totalCost << "\n";
                billFile.close();
                cout << "Report generated: billing_summary.txt\n";
                break;
            }
            case 6: { // Sync Delete
                string dName, n; double p, h;
                cout << "Enter name to remove: "; cin >> dName;
                ifstream fin("appliance.txt");
                ofstream fout("temp.txt");
                bool found = false;
                while (fin >> n >> p >> h) {
                    if (n != dName) fout << n << " " << p << " " << h << endl;
                    else found = true;
                }
                fin.close(); fout.close();
                remove("appliance.txt"); rename("temp.txt", "appliance.txt");
                
                if (found) {
                    for (auto it = appliances.begin(); it != appliances.end(); ++it) {
                        if (it->name == dName) { appliances.erase(it); break; }
                    }
                    cout << "Deleted successfully.\n";
                } else cout << "Appliance not found.\n";
                break;
            }
            case 7: { // Update Tariff
                cout << "Enter new tariff rate ($/kWh): ";
                if (cin >> tariff && tariff > 0) {
                    saveTariff(tariff);
                    cout << "Tariff saved to config.\n";
                } else { cout << "Invalid rate.\n"; clearBadInput(); }
                break;
            }
            case 8: { // Visual Bar Chart
                cout << "\n--- COST DISTRIBUTION ($) ---\n";
                for (const auto& app : appliances) {
                    double cost = app.energyKWhPerDay() * 30 * tariff;
                    cout << left << setw(12) << app.name << " | ";
                    for(int i = 0; i < (int)(cost * 2); i++) cout << "#"; // Scaled for visibility
                    cout << " ($" << fixed << setprecision(2) << cost << ")\n";
                }
                break;
            }
            default: cout << "Invalid selection.\n"; break;
        }
    }
    return 0;
}