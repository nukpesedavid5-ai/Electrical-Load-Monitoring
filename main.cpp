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
        cout << "Enter appliance name: ";
        getline(cin, a.name);
    } while (a.name.empty());

    while (true) {
        cout << "Enter power rating (W): ";
        cin >> a.powerW;
        if (!cin.fail() && a.powerW > 0) break;
        clearBadInput();
    }

    while (true) {
        cout << "Enter usage hours per day (0-24): ";
        cin >> a.hoursPerDay;
        if (!cin.fail() && a.hoursPerDay >= 0 && a.hoursPerDay <= 24) break;
        clearBadInput();
    }
    return a;
}

int main() {
    vector<Appliance> appliances;
    double tariff = 0.15; 

    while (true) {
        int choice = menu();
        switch (choice) {
            case 1: {
                Appliance a = registerAppliance();
                appliances.push_back(a);
                ofstream outFile("appliance.txt", ios::app);
                outFile << a.name << " " << a.powerW << " " << a.hoursPerDay << endl;
                outFile.close();
                cout << "Registered and saved to file!\n";
                break;
            }
            case 2: {
                cout << "\n--- Current Appliances ---\n";
                for(auto &app : appliances) cout << app.name << " (" << app.powerW << "W)\n";
                break;
            }
            case 5: {
    ifstream inputFile("appliance.txt");
    ofstream outputFile("billing_summary.txt");
    string appName; 
    double watts, hours;
    double totalMonthlyCost = 0;

    if (inputFile.is_open() && outputFile.is_open()) {
        outputFile << "=== ELECTRICAL BILLING SUMMARY ===\n";
        
        // This loop reads name, watts, and hours as separate units
        while (inputFile >> appName >> watts >> hours) {
            double dailyKwh = (watts * hours) / 1000.0;
            double monthlyCost = dailyKwh * 30 * tariff;
            totalMonthlyCost += monthlyCost;

            outputFile << left << setw(15) << appName << ": $" 
                       << fixed << setprecision(2) << monthlyCost << "/month\n";
        }

        outputFile << "----------------------------------\n";
        outputFile << "TOTAL ESTIMATED MONTHLY BILL: $" << totalMonthlyCost << "\n";
        
        inputFile.close();
        outputFile.close();
        cout << "Summary saved to billing_summary.txt\n";
    }
    break;
}
            case 6: {
                string deleteName, name; double w, h;
                cout << "Name to delete: "; cin >> deleteName;
                ifstream fin("appliance.txt");
                ofstream fout("temp.txt");
                bool found = false;
                while (fin >> name >> w >> h) {
                    if (name != deleteName) fout << name << " " << w << " " << h << endl;
                    else found = true;
                }
                fin.close(); fout.close();
                remove("appliance.txt"); rename("temp.txt", "appliance.txt");
                if(found) cout << "Deleted!\n"; else cout << "Not found.\n";
                break;
            }
            case 7: {
                cout << "New tariff: "; cin >> tariff;
                break;
            }
            case 8: {
                ifstream chartFile("appliance.txt");
                string name; double w, h;
                while (chartFile >> name >> w >> h) {
                    double cost = (w / 1000.0) * h * 30 * tariff;
                    cout << left << setw(10) << name << " | ";
                    for(int i = 0; i < (int)cost; i++) cout << "#";
                    cout << " ($" << cost << ")\n";
                }
                chartFile.close();
                break;
            }
            case 0: return 0;
        }
    }
}