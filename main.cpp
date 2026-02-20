#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <fstream>

using namespace std;

struct Appliance {
    string name;
    double powerW;
    double hoursPerDay;

    double energyKWhPerDay() const {
        return (powerW * hoursPerDay) / 1000.0;
    }
};

static void clearBadInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
    cout << "5. Billing summary (to file)\n";
    cout << "6. Delete appliance\n";
    cout << "7. Change electricity tariff\n";
    cout << "8. View usage bar chart\n";
    cout << "0. Exit\n";
    cout << "Choose: ";
    int choice;
    cin >> choice;
    if (cin.fail()) { clearBadInput(); return -1; }
    return choice;
}

Appliance registerAppliance() {
    Appliance a{};
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    do {
        cout << "Enter appliance name (use_underscores): ";
        getline(cin, a.name);
    } while (a.name.empty());

    while (true) {
        cout << "Enter power rating (W): ";
        cin >> a.powerW;
        if (!cin.fail() && a.powerW > 0) break;
        cout << "Invalid input. Enter a positive number.\n";
        clearBadInput();
    }

    while (true) {
        cout << "Enter usage hours per day (0-24): ";
        cin >> a.hoursPerDay;
        if (!cin.fail() && a.hoursPerDay >= 0 && a.hoursPerDay <= 24) break;
        cout << "Invalid input. Enter hours between 0 and 24.\n";
        clearBadInput();
    }
    return a;
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

    // Validation: Name must not be empty (Requirement 6)
    if (query.empty()) {
        cout << "Search term cannot be empty.\n";
        return;
    }

    string qLower = toLowerStr(query);
    bool found = false;

    for (const auto& a : appliances) {
        if (toLowerStr(a.name).find(qLower) != string::npos) {
            cout << "\nFound: " << a.name << " (" << a.powerW << "W)";
            found = true;
        }
    }
    if (!found) cout << "No match found.\n";
}

int main() {
    vector<Appliance> appliances;
    double tariff = 0.15; 

    // --- FIX 1: LOAD DATA ON STARTUP ---
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
        switch (choice) {
            case 1: {
                Appliance a = registerAppliance();
                appliances.push_back(a);
                ofstream outFile("appliance.txt", ios::app);
                if (outFile.is_open()) {
                    outFile << a.name << " " << a.powerW << " " << a.hoursPerDay << endl;
                    outFile.close();
                    cout << " successfully rigistered and Saved to file!\n";
                }
                break;
            }
            case 2: {
                if (appliances.empty()) {
                    cout << "No appliances registered.\n";
                } else {
                    cout << "\n--- Current Appliances ---\n";
                    cout << left << setw(15) << "Name" << setw(10) << "Power(W)" << "Hours/Day\n";
                    for(const auto &app : appliances) {
                        cout << left << setw(15) << app.name << setw(10) << app.powerW << app.hoursPerDay << endl;
                    }
                }
                break;
            }
            case 3: {
                searchApplianceByName(appliances);
                break;
            }
            case 4: {
                double totalKWh = 0;
                cout << "\n--- Daily Energy Consumption (kWh/day) ---\n";
                cout << left << setw(15) << "Appliance" << "Consumption\n";
                cout << "------------------------------------------\n";
                
                for (const auto& app : appliances) {
                    double dailyKWh = app.energyKWhPerDay();
                    totalKWh += dailyKWh;
                    cout << left << setw(15) << app.name << fixed << setprecision(3) << dailyKWh << " kWh\n";
                }
                
                cout << "------------------------------------------\n";
                cout << left << setw(15) << "TOTAL DAILY:" << totalKWh << " kWh\n";
                break;
            }
            case 5: {
                // Reads directly from the file to ensure the bill is accurate
                ifstream inputFile("appliance.txt");
                ofstream outputFile("billing_summary.txt");
                string appName; double watts, hours;
                double totalMonthlyCost = 0;

                if (inputFile.is_open() && outputFile.is_open()) {
                    outputFile << "=== ELECTRICAL BILLING SUMMARY ===\n";
                    while (inputFile >> appName >> watts >> hours) {
                        double monthlyCost = (watts * hours / 1000.0) * 30 * tariff;
                        totalMonthlyCost += monthlyCost;
                        outputFile << left << setw(15) << appName << ": $" 
                                   << fixed << setprecision(2) << monthlyCost << "/month\n";
                    }
                    outputFile << "----------------------------------\n";
                    outputFile << "TOTAL ESTIMATED MONTHLY BILL: $" << totalMonthlyCost << "\n";
                    inputFile.close(); outputFile.close();
                    cout << "Summary saved to billing_summary.txt\n";
                }
                break;
            }
            case 6: {
                // --- FIX 2: SYNC DELETION ---
                string deleteName, name; double w, h;
                cout << "Enter name to delete: "; cin >> deleteName;
                
                ifstream fin("appliance.txt");
                ofstream fout("temp.txt");
                bool found = false;

                while (fin >> name >> w >> h) {
                    if (name != deleteName) fout << name << " " << w << " " << h << endl;
                    else found = true;
                }
                fin.close(); fout.close();
                remove("appliance.txt"); rename("temp.txt", "appliance.txt");

                if (found) {
                    // Remove from vector so Option 2 stays updated
                    for (auto it = appliances.begin(); it != appliances.end(); ++it) {
                        if (it->name == deleteName) {
                            appliances.erase(it);
                            break;
                        }
                    }
                    cout << "Deleted successfully!\n";
                } else {
                    cout << "Appliance not found.\n";
                }
                break;
            }
            case 7: {
                cout << "Current tariff: $" << tariff << "/kWh. Enter new tariff: ";
                cin >> tariff;
                cout << "Tariff updated!\n";
                break;
            }
            case 8: {
                cout << "\n--- Monthly Cost Bar Chart ($) ---\n";
                for (const auto& app : appliances) {
                    double cost = (app.powerW * app.hoursPerDay / 1000.0) * 30 * tariff;
                    cout << left << setw(15) << app.name << " | ";
                    for(int i = 0; i < (int)cost; i++) cout << "#";
                    cout << " ($" << fixed << setprecision(2) << cost << ")\n";
                }
                break;
            }
            case 0: return 0;
            default: cout << "Invalid choice.\n";
        }
    }
}