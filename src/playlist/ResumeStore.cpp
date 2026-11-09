#include "ResumeStore.h"
#include "../core/Logger.h"

#include <sqlite3.h>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace Jonaki {

ResumeStore& ResumeStore::instance() {
    static ResumeStore s_instance;
    return s_instance;
}

ResumeStore::~ResumeStore() {
    close();
}

std::string ResumeStore::encodeKey(const std::string& path) const {
    // Percent encode matching PotPlayer style: e.g. "F%3a%5cFolder%5cFile.mkv"
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (unsigned char c : path) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << static_cast<int>(c);
        }
    }

    return escaped.str();
}

bool ResumeStore::initialize(const std::string& dbPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) return true;

    m_dbPath = dbPath;
    sqlite3* db = nullptr;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        Logger::error("Failed to open SQLite database: " + dbPath + " (" + sqlite3_errmsg(db) + ")");
        if (db) sqlite3_close(db);
        return false;
    }

    // Create table matching observed PotPlayer MediaInfo schema
    const char* schema = 
        "CREATE TABLE IF NOT EXISTS MediaInfo("
        "fn TEXT PRIMARY KEY, "
        "time INT, "
        "dur INT, "
        "fcc INT, "
        "width INT, "
        "height INT, "
        "fps FLOAT, "
        "arx INT, "
        "ary INT, "
        "vbit INT, "
        "tag INT, "
        "srate INT, "
        "ch INT, "
        "bits INT, "
        "abit INT, "
        "sub INT, "
        "fsize INT, "
        "dib BLOB, "
        "dsize INT);";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, schema, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        Logger::error("Failed to create MediaInfo table: " + std::string(errMsg ? errMsg : ""));
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    m_db = db;
    m_initialized = true;
    Logger::info("ResumeStore initialized successfully: " + dbPath);
    return true;
}

void ResumeStore::close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_db) {
        sqlite3_close(static_cast<sqlite3*>(m_db));
        m_db = nullptr;
    }
    m_initialized = false;
}

bool ResumeStore::savePosition(const std::string& filePath, int64_t positionMs, int64_t durationMs) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized || !m_db) return false;

    std::string key = encodeKey(filePath);
    const char* sql = "INSERT OR REPLACE INTO MediaInfo(fn, time, dur) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(static_cast<sqlite3*>(m_db), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, positionMs);
    sqlite3_bind_int64(stmt, 3, durationMs);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

bool ResumeStore::getPosition(const std::string& filePath, int64_t& outPositionMs, int64_t& outDurationMs) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized || !m_db) return false;

    std::string key = encodeKey(filePath);
    const char* sql = "SELECT time, dur FROM MediaInfo WHERE fn = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(static_cast<sqlite3*>(m_db), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        outPositionMs = sqlite3_column_int64(stmt, 0);
        outDurationMs = sqlite3_column_int64(stmt, 1);
        found = true;
    }

    sqlite3_finalize(stmt);
    return found;
}

bool ResumeStore::clearPosition(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_initialized || !m_db) return false;

    std::string key = encodeKey(filePath);
    const char* sql = "DELETE FROM MediaInfo WHERE fn = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(static_cast<sqlite3*>(m_db), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

} // namespace Jonaki
