#include <iostream>
#include <string>
#include <sqlite3.h>
using namespace std;


static int callback(void*, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; ++i)
        cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << '\n';
    cout << '\n';
    return 0;
}


void initDB() {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";

    if (sqlite3_open(path, &db) != SQLITE_OK) {
        cerr << "open error: " << sqlite3_errmsg(db) << '\n';
        sqlite3_close(db);
        return;
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS students ("
                  "id TEXT PRIMARY KEY, "
                  "name TEXT NOT NULL, "
                  "balance REAL DEFAULT 0.0);";
    char* err = nullptr;
    sqlite3_exec(db, sql, callback, nullptr, &err);
    if (err) { cerr << err << '\n'; sqlite3_free(err); }

    sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &err);
    if (err) { cerr << err << '\n'; sqlite3_free(err); }

    sqlite3_exec(db, "PRAGMA wal_checkpoint(FULL);", nullptr, nullptr, &err);
    if (err) sqlite3_free(err);

    sqlite3_close(db);
}


void DisplayStudentsFromDB() {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;

    sqlite3_stmt* stmt = nullptr;                     // NULL!
    const string sql = "SELECT name, id, balance FROM students;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "=== Students ===\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string id   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

            cout << "Name: "<< name << "\t | \t" << "ID: "<< id <<  '\n';
        }
        cout << "================\n";
    }
    if (stmt) sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void addStudentToDB(const string& name, const string& id, double balance) {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;

    sqlite3_stmt* stmt = nullptr;
    const string sql = "INSERT OR REPLACE INTO students (name, id, balance) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, id.c_str(),   -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 3, balance);
        sqlite3_step(stmt);
    }
    if (stmt) sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void deleteStudentFromDB(const string& id) {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;

    sqlite3_stmt* stmt = nullptr;
    const string sql = "DELETE FROM students WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    if (stmt) sqlite3_finalize(stmt);
    sqlite3_close(db);
}


void addMoneyToDB(const string& id, double amount) {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;

    sqlite3_stmt* stmt = nullptr;
    double cur = 0.0;


    const string sel = "SELECT balance FROM students WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sel.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) cur = sqlite3_column_double(stmt, 0);
    }
    if (stmt) sqlite3_finalize(stmt); stmt = nullptr;


    const string upd = "UPDATE students SET balance = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db, upd.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        double newBal = cur + amount;
        sqlite3_bind_double(stmt, 1, newBal);
        sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        cout << "Новый баланс: " << newBal << '\n';
    }
    if (stmt) sqlite3_finalize(stmt); stmt = nullptr;

    char* err = nullptr;
    sqlite3_exec(db, "PRAGMA wal_checkpoint(FULL);", nullptr, nullptr, &err);
    if (err) sqlite3_free(err);
    sqlite3_close(db);
}


void WithdrowMoneyFromDB(double amount, const string& id) {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;

    sqlite3_stmt* stmt = nullptr;
    double cur = 0.0;

    const string sel = "SELECT balance FROM students WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sel.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) cur = sqlite3_column_double(stmt, 0);
    }
    if (stmt) sqlite3_finalize(stmt); stmt = nullptr;

    const string upd = "UPDATE students SET balance = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db, upd.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        double new_Balance = cur - amount;
        if (new_Balance >= 0) {
            sqlite3_bind_double(stmt, 1, new_Balance);
            sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            cout << "Money has been withdraw, your new balance is: " << new_Balance << '\n';
        } else {
            cout << "You do not have such amount of money!\n";
        }
    }
    if (stmt) sqlite3_finalize(stmt); stmt = nullptr;

    char* err = nullptr;
    sqlite3_exec(db, "PRAGMA wal_checkpoint(FULL);", nullptr, nullptr, &err);
    if (err) sqlite3_free(err);
    sqlite3_close(db);
}


void ShowPI(const string& id) {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;

    sqlite3_stmt* stmt = nullptr;
    const string sql = "SELECT name, balance FROM students WHERE id = ?;";
    string name; double balance = 0.0;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            name    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            balance = sqlite3_column_double(stmt, 1);
        }
    }
    if (stmt) sqlite3_finalize(stmt); stmt = nullptr;

    if (!name.empty())
        cout << "=== Info ===\nName: " << name << "\nID: " << id << "\nBalance: " << balance << "\n============\n";

    char* err = nullptr;
    sqlite3_exec(db, "PRAGMA wal_checkpoint(FULL);", nullptr, nullptr, &err);
    if (err) sqlite3_free(err);
    sqlite3_close(db);
}
void Transaction(const string id,const string id1,double amount) {
    sqlite3* db = nullptr;
    const char* db_path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(db_path, &db) != SQLITE_OK) exit(1);
    sqlite3_stmt* stmt = nullptr;
    const string sql = "SELECT balance FROM students WHERE id = ?;";
    double current = 0.0;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) current = sqlite3_column_double(stmt, 0);
    }
    if (stmt) sqlite3_finalize(stmt); stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        double new_balance = current - amount;
        if (new_balance >= 0) {
            sqlite3_bind_double(stmt, 3, new_balance);
            sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            cout << "You succsefully sent money to the:" << id1 << endl;
            cout << "Your new balance: " << new_balance << '\n';
            cout << "You sent: " << amount << '\n';
        }
        else {
            cout << "You do not have such amount of money!\n";
        }
        sqlite3_finalize(stmt); stmt = nullptr;
    }
    const string upd = "UPDATE students SET balance = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db, upd.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        double new_Balance = current - amount;
        if (new_Balance >= 0) {
            sqlite3_bind_double(stmt, 1, new_Balance);
            sqlite3_bind_text(stmt, 2, id.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
        } else {
            cout << "You do not have such amount of money!\n";
        }
    }
    if (stmt) sqlite3_finalize(stmt); stmt = nullptr;

    char* err = nullptr;
    sqlite3_exec(db, "PRAGMA wal_checkpoint(FULL);", nullptr, nullptr, &err);

    double Current=0.0;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, id1.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) Current = sqlite3_column_double(stmt, 0);
    }
    if (stmt) sqlite3_finalize(stmt); stmt = nullptr;


    const string updd1 = "UPDATE students SET balance = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db, updd1.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        double new_Balance = Current + amount;
        if (new_Balance >= 0) {
            sqlite3_bind_double(stmt, 1, new_Balance);
            sqlite3_bind_text(stmt, 2, id1.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);

        }
        if (stmt) sqlite3_finalize(stmt); stmt = nullptr;


        sqlite3_exec(db, "PRAGMA wal_checkpoint(FULL);", nullptr, nullptr, &err);
        sqlite3_finalize(stmt); stmt = nullptr;
        sqlite3_close(db);
    }
}
void Hello(const string id) {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;

    sqlite3_stmt* stmt = nullptr;
    const string sql = "SELECT name FROM students WHERE id = ?;";
    string name;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            name    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        }
    }
    if (stmt) sqlite3_finalize(stmt); stmt = nullptr;

    if (!name.empty())
        cout << "Hello " << name << "!" << endl;

    char* err = nullptr;
    sqlite3_exec(db, "PRAGMA wal_checkpoint(FULL);", nullptr, nullptr, &err);
    if (err) sqlite3_free(err);
    sqlite3_close(db);
}


bool LogInStudent(const string& id) {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return 0;
    sqlite3_stmt* stmt = nullptr;
    const string sql = "SELECT id FROM students WHERE id = ?;";
    string id1;
    for (int i =1; i < 10; i++) {
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, i, id.c_str(), -1, SQLITE_STATIC);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                id1    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

            }
        }
        sqlite3_finalize(stmt); stmt = nullptr;
        if (id1 == id) {
            Hello(id);
            sqlite3_finalize(stmt); stmt = nullptr;
            sqlite3_close(db);
            return true;
        }
    }
    cout << "Invalid input, try again" << endl;
    sqlite3_finalize(stmt); stmt = nullptr;
    sqlite3_close(db);
    return false;
}

void initAdminDB() {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Administrators.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) {
        cerr << "Admin DB open error\n";
        return;
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS admins ("
                      "id TEXT PRIMARY KEY, "
                      "password TEXT NOT NULL);";
    char* err = nullptr;
    sqlite3_exec(db, sql, nullptr, nullptr, &err);
    if (err) { cerr << "Admin table error: " << err << '\n'; sqlite3_free(err); }

    sqlite3_exec(db, "PRAGMA wal_checkpoint(FULL);", nullptr, nullptr, &err);
    sqlite3_close(db);
    cout << "Admin DB ready!\n";
}

void addAdminToDB(const string& password, const string& id) {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Administrators.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;
    sqlite3_stmt* stmt = nullptr;
    const string sql = "INSERT OR REPLACE INTO admins (password, id) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, password.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, id.c_str(),   -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    if (stmt) sqlite3_finalize(stmt);
    sqlite3_close(db);
}
bool LogInAdmin(const string& id, const string& password) {
    sqlite3* db = nullptr;
    const char* db_path = "/Users/lukapashko/CLionProjects/FinalTestProject/Administrators.db";

    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        cerr << "Admin DB open error\n";
        return false;
    }

    sqlite3_stmt* stmt = nullptr;

    const string sql = "SELECT id, password FROM admins WHERE id = ?;";

    bool success = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {

        sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {

            const char* db_id       = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* db_password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

            if (db_id && db_password && id == db_id && password == db_password) {
                cout << "Admin logged in: " << id << endl;
                success = true;
            }
        }
    } else {
        cerr << "Prepare error: " << sqlite3_errmsg(db) << endl;
    }

    if (stmt) sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}
void DisplayAdminsFromDB() {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Administrators";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;
    sqlite3_stmt* stmt = nullptr;                     // NULL!
    const string sql = "SELECT name, id, balance FROM students;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "=== Students ===\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string id   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

            cout << password << " | " << id <<  '\n';
        }
        cout << "================\n";
    }
    if (stmt) sqlite3_finalize(stmt);
    sqlite3_close(db);
}
void DisplayStudentsFromDB1() {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Students.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;

    sqlite3_stmt* stmt = nullptr;                     // NULL!
    const string sql = "SELECT name, id, balance FROM students;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "=== Students ===\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string id   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string balance = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            cout << " Name: "<< name << "\t | \t " << " ID: "<< id <<  "\t | \t "<< " Balance: " << balance<< '\n';
        }
        cout << "================\n";
    }
    if (stmt) sqlite3_finalize(stmt);
    sqlite3_close(db);
}
void DisplayPassword() {
    sqlite3* db = nullptr;
    const char* path = "/Users/lukapashko/CLionProjects/FinalTestProject/Administrators.db";
    if (sqlite3_open(path, &db) != SQLITE_OK) return;
    sqlite3_stmt* stmt = nullptr;
    const string sql = "SELECT password FROM students;";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "=== Students ===\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            cout << "Password: "<< password <<  '\n';
        }
        cout << "================\n";
    }
    if (stmt) sqlite3_finalize(stmt);
    sqlite3_close(db);
}