#pragma once

#include <drogon/HttpController.h>
#include "../include/my_type.hpp"
#include "../include/my_db.hpp"
#include "../include/my_todo.hpp"
#include "../include/my_ssr.hpp"

using namespace drogon;

class TodoController : public drogon::HttpController<TodoController>
{
public:
    METHOD_LIST_BEGIN
    // POST /todos        -> 新規登録
    ADD_METHOD_TO(TodoController::create, "/api/todos", Post);
    ADD_METHOD_TO(TodoController::todo_create, "/api/todos/create", Post);
    // GET  /todos        -> 一覧取得
    ADD_METHOD_TO(TodoController::list, "/api/todos", Get);
    ADD_METHOD_TO(TodoController::todo_list, "/api/todos/list", Get);
    // DELETE /todos/{id} -> 削除
    ADD_METHOD_TO(TodoController::remove, "/todos/delete/{id}", Delete);
    ADD_METHOD_TO(TodoController::todo_remove, "/api/todos/delete", Post);
    ADD_METHOD_TO(TodoController::todo_get, "/api/todos/get/{id}", Get);
    METHOD_LIST_END

    void create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    void todo_create(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);

    void list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);
    void todo_list(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);

    void remove(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                int id);
    void todo_remove(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
    void todo_get(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                int id);

};
