#include <iostream>
#include <string>
#include <vector>
#include "db.cpp"
#include <cstdlib>
using namespace std;

int main() {
    initDB();

    // loadStudentsFromDB(students);
    DisplayAdminsFromDB();
    initAdminDB();
    int choice = 1;
    while (choice != 0) {
    cout << "1 to creat an account" << endl
         <<"2 to log in" << endl
         <<"3 log in as administrator" << endl
         <<"0 to exit: ";
    cin >> choice;
        system("clear");


        switch (choice) {
            case 1: {
                start:
                string name, id;

                double balance;
                int password;
                cout << "Enter name, id, balance and password: ";

                if (cin >> name >> id >> balance >> password) {
                    if (stoi(id)<1000 || stoi(id)>99999) {
                        cout << "Invalid input" << endl;
                        goto start;
                    }

                    addStudentToDB(name, id, balance);
                } else {
                    cout << "Invalid input! Please try again." << endl;
                    cin.clear();
                    cin.ignore(10000, '\n');
                }

                break;
            }
            case 2: {

                string id;

                do {

                    cout << "Enter id: ";
                    cin >> id;
                    if (stoi(id)<1000 || stoi(id)>99999) {
                        cout << "Invalid input" << endl;
                    }
                }while (stoi(id)<1000 || stoi(id)>999990);
                // for (auto& student : students) {
                //     student.check(id);
                // }
                bool j;
                int cho=1;
                if (LogInStudent(id)) {


                while (cho !=0) {
                    cout << "Enter 1 to add money" << endl
                    <<  "Enter 2 to do withdraw" << endl
                    << "Enter 3 to see your personal information" << endl
                    << "Enter 4 to send money" << endl
                    << "Enter 0 to log out: ";
                    cin >> cho;

                    switch (cho) {
                        case 1: {
                            cout << "Which amount of money do you want to add? " << endl;
                            double amount;
                            cin >> amount;
                            addMoneyToDB(id, amount);
                            break;

                        }
                        case 2: {
                            cout << "Which amount of money do you want to withdraw? " << endl;
                            double amount;
                            cin >> amount;
                            WithdrowMoneyFromDB(amount, id);

                            break;

                        }
                        case 3: {
                            ShowPI(id);
                            break;
                        }
                        case 4: {
                            cout << "On which ID do you want to money?" << endl;
                            DisplayStudentsFromDB();
                            string id1;
                            cin >> id1;
                            cout << "What amount of money do you want to send?" << endl;
                            double amount;
                            cin >> amount;
                            Transaction(id,id1, amount);

                            break;
                        }
                        case 5: {
                            DisplayStudentsFromDB();
                        }
                        case 0: {

                        }
                        default: {
                            break;
                        }
                    }
                    }

                }

            break;
            }
            default: {
                cout << "Invalid choice! Try again." << endl;
                break;
            }
            case 3: {
                cout << "Enter id and password: ";
                string ida, password;
                cin >> ida >> password;
                if (LogInAdmin(ida, password)) {
                    cout << "Successfully logged in." << endl;
                    int g=1;
                    while (g) {

                        cout << "1.Delete Student" << endl
                             << "2.Show Students" << endl
                             << "0. Log out" << endl;
                        cin >> g;
                        switch (g) {
                            case 1: {
                                string id;
                                cout << "Enter id to delete: ";
                                cin >> id;
                                deleteStudentFromDB(id);
                                // Удаляем из вектора
                                // for (auto it = students.begin(); it != students.end(); ++it) {
                                //     if (it->getId() == id) {
                                //         students.erase(it);
                                //         break;
                                //     }
                                // }
                                break;
                            }
                            case 2: {
                                DisplayStudentsFromDB1();
                                break;
                            }
                            default:
                                ;
                        }
                    }

                }
            }

        }


    }
    return 0;
}