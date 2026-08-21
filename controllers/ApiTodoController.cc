#include "ApiTodoController.h"

using namespace drogon::orm;

// ------------------------------------------------------------
// POST /todos  { "title": "牛乳を買う" }
// ------------------------------------------------------------
void TodoController::create(const HttpRequestPtr &req,
                             std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("title") || (*json)["title"].asString().empty())
    {
        Json::Value err;
        err["error"] = "title is required";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    std::string title = (*json)["title"].asString();
    auto dbClient = app().getDbClient();

    try
    {
        dbClient->execSqlSync("INSERT INTO todos (title) VALUES (?)", title);
        auto result = dbClient->execSqlSync("SELECT last_insert_rowid() AS id");
        long long id = result[0]["id"].as<long long>();

        Json::Value ret;
        ret["id"] = static_cast<Json::Int64>(id);
        ret["title"] = title;
        ret["done"] = false;

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(k201Created);
        callback(resp);
    }
    catch (const DrogonDbException &e)
    {
        Json::Value err;
        err["error"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void TodoController::todo_create(const HttpRequestPtr &req,
                             std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto title = req->getParameter("title");
    if (title.empty())
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("title is required");
        callback(resp);        
        return;
    }
    auto dbClient = app().getDbClient();

    try
    {
        MyTodo tLib("");
        tLib.todo_add_handler(title);
        MyDb dLib(DB_PATH);
        auto todos = dLib.list("all"); 
        MySsr sLib("");       
        std::string htm = sLib.renderTodoList(todos);

        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(htm);
        callback(resp);        
    }
    catch (const DrogonDbException &e)
    {
        Json::Value err;
        err["error"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

// ------------------------------------------------------------
// GET /todos  -> 一覧を新しい順に返す
// ------------------------------------------------------------
void TodoController::list(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = app().getDbClient();

    try
    {
        auto result = dbClient->execSqlSync(
            "SELECT id, title, done, created_at FROM todos ORDER BY id DESC");

        Json::Value arr(Json::arrayValue);
        for (const auto &row : result)
        {
            Json::Value item;
            item["id"] = static_cast<Json::Int64>(row["id"].as<long long>());
            item["title"] = row["title"].as<std::string>();
            item["done"] = row["done"].as<int>() != 0;
            item["created_at"] = row["created_at"].as<std::string>();
            arr.append(item);
        }

        auto resp = HttpResponse::newHttpJsonResponse(arr);
        callback(resp);
    }
    catch (const DrogonDbException &e)
    {
        Json::Value err;
        err["error"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void TodoController::todo_list(const HttpRequestPtr &req,
                           std::function<void(const HttpResponsePtr &)> &&callback)
{
    try
    {
        MyDb dLib(DB_PATH);
        auto todos = dLib.list("all"); 
        MySsr sLib("");       
        std::string htm = sLib.renderTodoList(todos);
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(htm);
        callback(resp);
    }
    catch (const DrogonDbException &e)
    {
        Json::Value err;
        err["error"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

// ------------------------------------------------------------
// DELETE /todos/{id}
// ------------------------------------------------------------
void TodoController::remove(const HttpRequestPtr &req,
                             std::function<void(const HttpResponsePtr &)> &&callback,
                             int id)
{
    auto dbClient = app().getDbClient();

    try
    {
        auto result = dbClient->execSqlSync("DELETE FROM todos WHERE id = ?", id);

        Json::Value ret;
        if (result.affectedRows() > 0)
        {
            ret["message"] = "deleted";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
        else
        {
            ret["error"] = "todo not found";
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k404NotFound);
            callback(resp);
        }
    }
    catch (const DrogonDbException &e)
    {
        Json::Value err;
        err["error"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void TodoController::todo_remove(const HttpRequestPtr &req,
                             std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = app().getDbClient();

    try
    {
        auto id = req->getParameter("id");
        if (id.empty())
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody("id is required");
            callback(resp);        
            return;
        }        
        MyTodo tLib("");
        tLib.todo_delete_handler(std::stoi(id));
        MyDb dLib(DB_PATH);
        auto todos = dLib.list("all"); 
        MySsr sLib("");       
        std::string htm = sLib.renderTodoList(todos);

        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(htm);
        callback(resp);        
    }
    catch (const DrogonDbException &e)
    {
        Json::Value err;
        err["error"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void TodoController::todo_get(const HttpRequestPtr &req,
                             std::function<void(const HttpResponsePtr &)> &&callback,
                             int id)
{
    try
    {
        auto resp = HttpResponse::newHttpResponse();
        MyTodo tLib("");
        std::string htm = tLib.dialog_show(id);
        //resp->setBody("id" + std::to_string(id));
        resp->setBody(htm);
        callback(resp);  
        return;
    }
    catch (const DrogonDbException &e)
    {
        Json::Value err;
        err["error"] = e.base().what();
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}