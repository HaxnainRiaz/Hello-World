#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <windows.h>

using namespace std;

struct Transaction {
    string type; 
    double amount;
    string timestamp;
};

struct Account {
    string id;
    string username;
    string password;
    string accountType;
    string dateOfBirth;
    string dateCreated;
    double balance;
    vector<Transaction> transactions;
    Account* prev;
    Account* next;
};

class BankSystem {
private:
    Account* head;
    Account* tail;

    string getCurrentDate() {
        time_t now = time(nullptr);
        tm* localTime = localtime(&now);
        stringstream date;
        date << (localTime->tm_mday) << "/"
             << (localTime->tm_mon + 1) << "/"
             << (localTime->tm_year + 1900);
        return date.str();
    }

    string generateUniqueId() {
        srand(time(0));
        int id = rand() % 9000 + 1000; 
        return to_string(id);
    }

public:
    BankSystem() : head(nullptr), tail(nullptr) {}

    void addAccount(string username, string password, string accountType, string dob, double deposit) {
        Account* newAccount = new Account();
        newAccount->id = generateUniqueId();
        newAccount->username = username;
        newAccount->password = password;
        newAccount->accountType = accountType;
        newAccount->dateOfBirth = dob;
        newAccount->dateCreated = getCurrentDate();
        newAccount->balance = deposit;
        newAccount->transactions.push_back({"Deposit", deposit, getCurrentDate()});
        newAccount->prev = nullptr;
        newAccount->next = nullptr;

        if (head == nullptr) {
            head = tail = newAccount;
        } else {
            tail->next = newAccount;
            newAccount->prev = tail;
            tail = newAccount;
        }

        saveToFile();
        cout << "\t\t\t\t\t\tYour account has been created successfully!\n"
        << "\t\t\t\t\t\tYour account ID is: " << newAccount->id << endl;
    }

    bool validatePin(const string& pin) {
        if (pin.length() != 4) return false;
        for (char c : pin) {
            if (!isdigit(c)) return false;
        }
        for (size_t i = 0; i < pin.length(); ++i) {
            for (size_t j = i + 1; j < pin.length(); ++j) {
                if (pin[i] == pin[j]) return false;
            }
        }
        return true;
    }

    bool isValidAmount(const string& amountStr) {
        for (char c : amountStr) {
            if (!isdigit(c) && c != '.') return false;
        }
        double amount = stod(amountStr);
        return amount > 0;
    }

    void deposit(string id, const string& amountStr) {
        if (!isValidAmount(amountStr)) {
            cout << "\t\t\t\t\t\tInvalid amount! Please enter a valid number greater than 0.\n";
            return;
        }
        double amount = stod(amountStr);
        Account* temp = head;
        while (temp != nullptr) {
            if (temp->id == id) {
                temp->balance += amount;
                temp->transactions.push_back({"Deposit", amount, getCurrentDate()});
                cout << "\t\t\t\t\t\tDeposit of $" << fixed << setprecision(2) << amount << " successful!\n";
                saveToFile(); // Save updated data
                return;
            }
            temp = temp->next;
        }
        cout << "\t\t\t\t\t\tAccount not found!\n";
    }

    void withdraw(string id, const string& amountStr) {
        if (!isValidAmount(amountStr)) {
            cout << "\t\t\t\t\t\tInvalid amount! Please enter a valid number greater than 0.\n";
            return;
        }
        double amount = stod(amountStr);
        Account* temp = head;
        while (temp != nullptr) {
            if (temp->id == id) {
                if (temp->balance >= amount) {
                    temp->balance -= amount;
                    temp->transactions.push_back({"Withdraw", amount, getCurrentDate()});
                    cout << "\t\t\t\t\t\tWithdrawal of $" << fixed << setprecision(2) << amount << " successful!\n";
                    saveToFile(); // Save updated data
                } else {
                    cout << "\t\t\t\t\t\tInsufficient funds! Withdrawal failed.\n";
                }
                return;
            }
            temp = temp->next;
        }
        cout << "\t\t\t\t\t\tAccount not found!\n";
    }

    void displayAccountInfo(string id) {
        Account* temp = head;
        while (temp != nullptr) {
            if (temp->id == id) {
                cout << "\t\t\t\t\t\t====================================\n";
                cout << "\t\t\t\t\t\t       Account Information\n";
                cout << "\t\t\t\t\t\t====================================\n";
                cout << "\t\t\t\t\t\tAccount ID: " << temp->id << endl;
                cout << "\t\t\t\t\t\tUsername: " << temp->username << endl;
                cout << "\t\t\t\t\t\tAccount Type: " << temp->accountType << endl;
                cout << "\t\t\t\t\t\tDate of Birth: " << temp->dateOfBirth << endl;
                cout << "\t\t\t\t\t\tDate Created: " << temp->dateCreated << endl;
                cout << "\t\t\t\t\t\tCurrent Balance: $" << fixed << setprecision(2) << temp->balance << endl;
                cout << "\t\t\t\t\t\t====================================\n";
                return;
            }
            temp = temp->next;
        }
        cout << "\t\t\t\t\t\tAccount not found!\n";
    }
    void BalanceEnquiry(string id) {
        Account* temp = head;
        while (temp != nullptr) {
            if (temp->id == id) {
                cout << "\t\t\t\t\t\t====================================\n";
                cout << "\t\t\t\t\t\t       Account Information\n";
                cout << "\t\t\t\t\t\t====================================\n";
                cout << "\n\n";
                cout << "\t\t\t\t\t\tCurrent Balance: $" << fixed << setprecision(2) << temp->balance << endl;
                cout << "\t\t\t\t\t\t====================================\n";
                return;
            }
            temp = temp->next;
        }
        cout << "\t\t\t\t\t\tAccount not found!\n";
    }

    void viewTransactionHistory(string id) {
        Account* temp = head;
        while (temp != nullptr) {
            if (temp->id == id) {
                cout << "\t\t\t\t\t\t====================================\n";
                cout << "\t\t\t\t\t\t        Transaction History\n";
                cout << "\t\t\t\t\t\t====================================\n";
                cout << "\t\t\t\t\t\tType           Amount          Date\n";
                cout << "\t\t\t\t\t\t------------------------------------\n";
                for (const auto& transaction : temp->transactions) {
                    cout << "\t\t\t\t\t\t" << setw(10) << left << transaction.type
                         << setw(15) << right << transaction.amount
                         << "   " << transaction.timestamp << endl;
                }
                cout << "\t\t\t\t\t\t====================================\n";
                return;
            }
            temp = temp->next;
        }
        cout << "\t\t\t\t\t\tAccount not found!\n";
    }

    void deleteAccount(string id, string password) {
        Account* temp = head;
        while (temp != nullptr) {
            if (temp->id == id && temp->password == password) {
                if (temp == head) {
                    head = temp->next;
                    if (head != nullptr) head->prev = nullptr;
                } else if (temp == tail) {
                    tail = temp->prev;
                    if (tail != nullptr) tail->next = nullptr;
                } else {
                    temp->prev->next = temp->next;
                    temp->next->prev = temp->prev;
                }
                delete temp;
                cout << "\t\t\t\t\t\tAccount deleted successfully!\n";
                saveToFile();
                return;
            }
            temp = temp->next;
        }
        cout << "\t\t\t\t\t\tIncorrect account details!\n";
    }

    void saveToFile() {
        ofstream file("accounts.txt", ios::trunc);
        Account* temp = head;
        while (temp != nullptr) {
            file << temp->id << endl;
            file << temp->username << endl;
            file << temp->password << endl;
            file << temp->accountType << endl;
            file << temp->dateOfBirth << endl;
            file << temp->dateCreated << endl;
            file << temp->balance << endl;
            for (const auto& transaction : temp->transactions) {
                file << transaction.type << "," << transaction.amount << "," << transaction.timestamp << "|";
            }
            file << endl;
            temp = temp->next;
        }
        file.close();
    }

    void loadFromFile() {
        ifstream file("accounts.txt");
        string id, username, password, accountType, dob, dateCreated;
        double balance;
        string transactionData;
        while (getline(file, id) && getline(file, username) && getline(file, password) && getline(file, accountType) && getline(file, dob) && getline(file, dateCreated) && file >> balance) {
            file.ignore();
            Account* newAccount = new Account();
            newAccount->id = id;
            newAccount->username = username;
            newAccount->password = password;
            newAccount->accountType = accountType;
            newAccount->dateOfBirth = dob;
            newAccount->dateCreated = dateCreated;
            newAccount->balance = balance;
            getline(file, transactionData);
            stringstream ss(transactionData);
            string transaction;
            while (getline(ss, transaction, '|')) {
                if (transaction.empty()) continue;
                stringstream ts(transaction);
                string type, date;
                double amount;
                getline(ts, type, ',');
                ts >> amount;
                getline(ts, date, ',');
                newAccount->transactions.push_back({type, amount, date});
            }
            if (head == nullptr) {
                head = tail = newAccount;
            } else {
                tail->next = newAccount;
                newAccount->prev = tail;
                tail = newAccount;
            }
        }
        file.close();
    }

    bool login(string id, string username, string password) {
        Account* temp = head;
        while (temp != nullptr) {
            if (temp->id == id && temp->username == username && temp->password == password) {
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    void displayAllAccounts() {
        if (head == nullptr) {
            cout << "\t\t\t\t\t\tNo accounts found!\n";
            return;
        }
        Account* temp = head;
        cout << "\t\t\t\t\t================================================\n";
        cout << "\t\t\t\t\t              List of All Accounts\n";
        cout << "\t\t\t\t\t================================================\n";
        while (temp != nullptr) {
            cout << "\t\t\t\t\tUsername: " << temp->username << ", Account Type: " << temp->accountType << ", Balance: $" << fixed << setprecision(2) << temp->balance << endl;
            temp = temp->next;
        }
        cout << "\t\t\t\t\t================================================\n";
    }
};

void clearScreen() {
    COORD topLeft  = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}


bool isValidChoice(const string& input) {
    if (input.length() != 1 || !isdigit(input[0])) {
        return false;
    }
    int choice = input[0] - '0';
    return choice >= 1 && choice <= 7;
}

bool isValidDate(const string& date) {
    if (date.length() != 10) return false;
    if (date[2] != '/' || date[5] != '/') return false;
    for (size_t i = 0; i < date.length(); ++i) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(date[i])) return false;
    }
    int day = stoi(date.substr(0, 2));
    int month = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 || year > 2007) return false;
    return true;
}
bool isvalidtype(const string& accountType) {
    if (accountType == "Saving" || accountType == "Current") {
        return true;
    }
    return false;
}

bool isValidDepositAmount(const string& amountStr) {
    for (char c : amountStr) {
        if (!isdigit(c) && c != '.') return false;
    }
    double amount = stod(amountStr);
    return amount > 0;
}

int main() {
    BankSystem bank;
    bank.loadFromFile();
    string account;
    string menuInput;
    int choice;
    string id, username, password;
    string accountType;
    string dob;
    double depositAmount;

    while (true) {
        clearScreen();
        cout << string(1, '\n');
        cout << "\t\t\t\t\t\t====================================\n";
        cout << "\t\t\t\t\t\t      Welcome to the Bank System\n";
        cout << "\t\t\t\t\t\t====================================\n";
        cout << "\n\n";
        cout << "\t\t\t\t\t\t1. Login\n";
        cout << "\t\t\t\t\t\t2. Register\n";
        cout << "\t\t\t\t\t\t3. View All Accounts\n";
        cout << "\t\t\t\t\t\t4. Exit\n";
        cout << "\t\t\t\t\t\tEnter choice: ";
        cin >> menuInput;
        
        while (!isValidChoice(menuInput)) {
            cout << "\t\t\t\t\t\tInvalid input! Please enter a number between 1 and 4.\n";
            cout << "\t\t\t\t\t\tEnter choice: ";
            cin>> menuInput;
        }

        choice = menuInput[0] - '0';

        if (choice == 1) {
            clearScreen();
            int loginAttempts = 0;
            bool loggedIn = false;
            while (loginAttempts < 2 && !loggedIn) {
                cout << "\t\t\t\t\t\t====================================\n";
                cout << "\t\t\t\t\t\t               Login\n";
                cout << "\t\t\t\t\t\t====================================\n";
                cout << "\n\n";             
                cout << "\t\t\t\t\t\tEnter Account ID: ";
                cin >> id;
                cout << "\t\t\t\t\t\tEnter Username: ";
                cin >> username;
                cout << "\t\t\t\t\t\tEnter Password: ";
                cin >> password;

                if (bank.login(id, username, password)) {
                    loggedIn = true;
                             
                    string actionInput;
                    int action;
                    while (true) {
                        clearScreen();
                        cout << "\t\t\t\t\t\t====================================\n";
                        cout << "\t\t\t\t\t\t      Welcome" << username << "!\n";
                        cout << "\t\t\t\t\t\t====================================\n";
                        cout << "\n\n";
                        cout << "\t\t\t\t\t\t1. Deposit\n";
                        cout << "\t\t\t\t\t\t2. Withdraw\n";
                        cout << "\t\t\t\t\t\t3. Balance Inquiry\n";
                        cout << "\t\t\t\t\t\t4. Delete Account\n";
                        cout << "\t\t\t\t\t\t5. View Account Info\n";
                        cout << "\t\t\t\t\t\t6. View Transaction History\n";
                        cout << "\t\t\t\t\t\t7. Logout\n";
                        cout << "\t\t\t\t\t\tEnter choice: ";
                        cin >> actionInput;

                        while (!isValidChoice(actionInput) || actionInput[0] < '1' || actionInput[0] > '7') {
                            cout << "\t\t\t\t\t\tInvalid input! Please enter a number between 1 and 7.\n";
                            cout << "\t\t\t\t\t\tPress Enter to try again...";
                            cin.ignore(); // Clear the input buffer
                            cin.get(); // Wait for Enter key
                            clearScreen();
                        cout << "\t\t\t\t\t\t====================================\n";
                        cout << "\t\t\t\t\t\t      Welcome" << username << "!\n";
                        cout << "\t\t\t\t\t\t====================================\n";
                        cout << "\n\n";
                        cout << "\t\t\t\t\t\t1. Deposit\n";
                        cout << "\t\t\t\t\t\t2. Withdraw\n";
                        cout << "\t\t\t\t\t\t3. Balance Inquiry\n";
                        cout << "\t\t\t\t\t\t4. Delete Account\n";
                        cout << "\t\t\t\t\t\t5. View Account Info\n";
                        cout << "\t\t\t\t\t\t6. View Transaction History\n";
                        cout << "\t\t\t\t\t\t7. Logout\n";
                        cout << "\t\t\t\t\t\tEnter choice: ";
                            cin >> actionInput;
                        }

                        action = actionInput[0] - '0';

                        if (action == 1) {
                            string depositStr;
                            cout << "\t\t\t\t\t\tEnter deposit amount: ";
                            cin >> depositStr;
                            bank.deposit(id, depositStr);
                            cin.ignore();
                            cin.get();
                        } else if (action == 2) {
                            string withdrawStr;
                            cout << "\t\t\t\t\t\tEnter withdraw amount: ";
                            cin >> withdrawStr;
                            bank.withdraw(id, withdrawStr);
                            cin.ignore();
                            cin.get();
                        } else if (action == 3) {
                            bank.BalanceEnquiry(id);
                            cin.ignore();
                            cin.get();
                        } else if (action == 4) {
                            bank.deleteAccount(id, password);
                            break;
                        } else if (action == 5) {
                            bank.displayAccountInfo(id);
                            cin.ignore();
                            cin.get();
                        } else if (action == 6) {
                            bank.viewTransactionHistory(id);
                            cin.ignore();
                            cin.get();
                        } else if (action == 7) {
                            break;
                        } else {
                            cout << "\t\t\t\t\t\tInvalid choice! Please try again.\n";
                            cin.ignore();
                            cin.get();
                        }
                    }
                } else {
                    loginAttempts++;
                    if (loginAttempts < 2) {
                        cout << "\t\t\t\t\t\tInvalid login credentials! Please try again.\n";
                    } else {
                        cout << "\t\t\t\t\t\tMaximum login attempts reached!\n";
                        cin.ignore();
                        cin.get();
                    }
                }
            }
        } else if (choice == 2) {
            clearScreen();
            cout << "\t\t\t\t\t\t====================================\n";
            cout << "\t\t\t\t\t\t      Register a New Account\n";
            cout << "\t\t\t\t\t\t====================================\n";
            cout << "\n\n";
            cout << "\t\t\t\t\t\tEnter Username: ";
            cin >> username;
            cout << "\t\t\t\t\t\tEnter 4-digit PIN (no repeating digits): ";
            cin >> password;
            if (!bank.validatePin(password)) {
                cout << "\t\t\t\t\t\tInvalid PIN!Press Enter to exit \n";
                cin.ignore();
                cin.get();
            }
            cout << "\t\t\t\t\t\tEnter Account Type: ";
            cin >> accountType;
            if(!isvalidtype(accountType)){
                cout << "\t\t\t\t\t\tInvalid Account Type!Press Enter to exit \n";
                cin.ignore();
                cin.get();
            }
            
            cout << "\t\t\t\t\t\tEnter Date of Birth (dd/mm/yyyy): ";
            cin >> dob;
            if (!isValidDate(dob)) {
                cout << "\t\t\t\t\t\tInvalid date format! \n";
                cout << "\t\t\t\t\t\tEnter Date of Birth (dd/mm/yyyy): ";
                cin >> dob;
            }
            cout << "\t\t\t\t\t\tEnter initial deposit amount: ";
            cin >> depositAmount;
            bank.addAccount(username, password, accountType, dob, depositAmount);
            cin.ignore();
            cin.get();
     } else if (choice == 3) {
            clearScreen();
            bank.displayAllAccounts();
            cin.ignore();
            cin.get();
        } else if (choice == 4) {
            clearScreen();
            cout << "\t\t\t\t\t\tExiting the program...\n";
            break;
        }
    }
    return 0;
}
