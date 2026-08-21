#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class MyDb {
private:
    std::string m_name;
    sqlite3* db_ = nullptr;

    void exec(const std::string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            std::string msg = err ? err : "unknown";
            sqlite3_free(err);
            die(msg);
        }
    }

    void prepare(const std::string& sql, sqlite3_stmt** s) {
        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, s, nullptr) != SQLITE_OK)
            die(sqlite3_errmsg(db_));
    }

    void step_and_finalize(sqlite3_stmt* s) {
        sqlite3_step(s);
        sqlite3_finalize(s);
    }

    static std::string timestamp() {
        std::time_t t = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return buf;
    }

    [[noreturn]] static void die(const std::string& msg) {
        std::cerr << "DB error: " << msg << "\n";
        std::exit(1);
    }

public:
    explicit MyDb(const std::string& path){
        if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK)
            die("open");
        exec("PRAGMA journal_mode=WAL;");
        exec(R"(
            CREATE TABLE IF NOT EXISTS todos (
                id         INTEGER PRIMARY KEY AUTOINCREMENT,
                title      TEXT    NOT NULL,
                done       INTEGER NOT NULL DEFAULT 0,
                created_at TEXT    NOT NULL
            );
        )");
    }

    ~MyDb() { sqlite3_close(db_); }


    // ── Write ──────────────────────────────
    void add(const std::string& title) {
        std::string now = timestamp();
        sqlite3_stmt* s;
        prepare("INSERT INTO todos (title, done, created_at) VALUES (?, 0, ?);", &s);
        sqlite3_bind_text(s, 1, title.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(s, 2, now.c_str(),   -1, SQLITE_TRANSIENT);
        step_and_finalize(s);
        std::cout << "✓ 追加しました: [" << sqlite3_last_insert_rowid(db_) << "] " << title << "\n";
    }

    void done(int id) {
        sqlite3_stmt* s;
        prepare("UPDATE todos SET done = 1 WHERE id = ?;", &s);
        sqlite3_bind_int(s, 1, id);
        step_and_finalize(s);
        if (sqlite3_changes(db_) == 0)
            std::cout << "ID " << id << " が見つかりません。\n";
        else
            std::cout << "✓ 完了しました: ID " << id << "\n";
    }

    void undone(int id) {
        sqlite3_stmt* s;
        prepare("UPDATE todos SET done = 0 WHERE id = ?;", &s);
        sqlite3_bind_int(s, 1, id);
        step_and_finalize(s);
        if (sqlite3_changes(db_) == 0)
            std::cout << "ID " << id << " が見つかりません。\n";
        else
            std::cout << "✓ 未完了に戻しました: ID " << id << "\n";
    }

    void remove(int id) {
        sqlite3_stmt* s;
        prepare("DELETE FROM todos WHERE id = ?;", &s);
        sqlite3_bind_int(s, 1, id);
        step_and_finalize(s);
        if (sqlite3_changes(db_) == 0)
            std::cout << "ID " << id << " が見つかりません。\n";
        else
            std::cout << "✓ 削除しました: ID " << id << "\n";
    }

    void clear_done() {
        exec("DELETE FROM todos WHERE done = 1;");
        std::cout << "✓ 完了済みタスクをすべて削除しました。\n";
    }

    // ── Read ───────────────────────────────
    std::vector<Todo> list(const std::string& filter = "all") {
        std::string sql = "SELECT id, title, done, created_at FROM todos";
        if (filter == "pending")  sql += " WHERE done = 0";
        if (filter == "done")     sql += " WHERE done = 1";
        sql += " ORDER BY id;";

        sqlite3_stmt* s;
        prepare(sql, &s);
        std::vector<Todo> rows;
        while (sqlite3_step(s) == SQLITE_ROW) {
            rows.push_back({
                sqlite3_column_int (s, 0),
                reinterpret_cast<const char*>(sqlite3_column_text(s, 1)),
                sqlite3_column_int (s, 2) != 0,
                reinterpret_cast<const char*>(sqlite3_column_text(s, 3))
            });
        }
        sqlite3_finalize(s);
        return rows;
    }    

    std::string get_list_json(const std::vector<Todo>& todos) {
        std::string ret = "";
        std::vector<Todo> items;
        if (todos.empty()) {
            std::cout << "  (タスクはありません)\n";
            return "[]";
        }
        for (const auto& t : todos) {
            Todo todoRow;
            todoRow.id = t.id;
            todoRow.title = t.title;
            todoRow.done = false;
            todoRow.created_at = "";
            items.push_back(todoRow);
        }
        json j1 = items;
        std::string json_str = j1.dump();
        //std::cout << "\n";
        ret = json_str;
        return ret;
    }    

    // ─────────────────────────────────────────
    //  Display
    // ─────────────────────────────────────────
    void print_table(const std::vector<Todo>& todos) {
        if (todos.empty()) {
            std::cout << "  (タスクはありません)\n";
            return;
        }
        // header
        std::cout << "\n"
                << "  " << std::left
                << std::setw(5)  << "ID"
                << std::setw(6)  << "状態"
                << std::setw(40) << "タイトル"
                << "作成日時" << "\n"
                << "  " << std::string(70, '-') << "\n";

        for (const auto& t : todos) {
            std::string status = t.done ? "✔" : "○";
            std::string title  = t.title;
            if (title.size() > 37) title = title.substr(0, 34) + "...";
            std::cout << "  "
                    << std::setw(5)  << t.id
                    << std::setw(6)  << status
                    << std::setw(40) << title
                    << t.created_at << "\n";
        }
        std::cout << "\n";
    }    

};
