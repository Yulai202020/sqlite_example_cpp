#include <iostream>
#include <sqlite3.h> 
#include <readline/readline.h>
#include <readline/history.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}

int request(sqlite3 *db, std::string sql, char *zErrMsg) {
    const char *cstr = sql.c_str();
    int rc = sqlite3_exec(db, cstr, callback, 0, &zErrMsg);
   
    if( rc != SQLITE_OK ){
        std::cerr << "SQL error: " << zErrMsg << "." << std::endl;
        sqlite3_free(zErrMsg);
        return 0;
    } else {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    // create smth
    int rc;
    sqlite3 *db;
    char *zErrMsg;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <sqlite_db>" << std::endl;
        return 0;
    }

    // open database
    rc = sqlite3_open(argv[1], &db);

    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << "." << std::endl;
        return 0;
    }

    // create sql request
    std::string all;

    while (1) {
        char *str = readline(": ");
        all += str;

        std::size_t found = all.find(";");
        if (found != std::string::npos) {
            int res = request(db, all, zErrMsg);
            all = "";
        }
    }

    // close db
    sqlite3_close(db);
    return 0;
}