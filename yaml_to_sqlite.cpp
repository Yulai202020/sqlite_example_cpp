#include <iostream>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>
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
        return SQLITE_OK;
    }

    return 0;
}

int createTable(sqlite3* db) {
    char* errMsg = nullptr;
    int rc = request(db, "CREATE TABLE IF NOT EXISTS people (id INTEGER PRIMARY KEY, name TEXT, age INTEGER, city TEXT);", errMsg);
    free(errMsg);
    return rc;
}

int insertData(sqlite3* db, int id, const std::string& name, int age, const std::string& city) {
    char* errMsg = nullptr;
    std::string sql = "INSERT INTO people (id, name, age, city) VALUES (" + std::to_string(id) + ", '" + name + "', " + std::to_string(age) + ", '" + city + "');";

    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return rc;
    }

    free(errMsg);

    return SQLITE_OK;
}

void parseYamlAndInsert(sqlite3* db, const std::string& filename) {
    YAML::Node data = YAML::LoadFile(filename);

    for (const auto& person : data) {
        int id = person["id"].as<int>();
        std::string name = person["name"].as<std::string>();
        int age = person["age"].as<int>();
        std::string city = person["city"].as<std::string>();

        insertData(db, id, name, age, city);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <yaml_file> <sqlite_db>" << std::endl;
        return 0;
    }

    const std::string filename_yaml = argv[1];
    const char* filename_db = argv[2];
    sqlite3* db = nullptr;

    int rc = sqlite3_open(filename_db, &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    rc = createTable(db);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return rc;
    }

    parseYamlAndInsert(db, filename_yaml);

    sqlite3_close(db);

    free(db);
    return 0;
}
