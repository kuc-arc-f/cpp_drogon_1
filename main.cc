#include <drogon/drogon.h>
#include "./include/my_todo.hpp"
#include "./include/my_ssr.hpp"

using namespace drogon;

int main() {
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 5555);

    app().registerHandler(
        "/", [](const HttpRequestPtr &request,  std::function<void(const HttpResponsePtr &)> &&callback) {
            std::string htm = ssr_htm_top();
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(htm);
            callback(resp);
        }, {Get});

    app().registerHandler(
        "/api/todo/list", [](const HttpRequestPtr &request,  std::function<void(const HttpResponsePtr &)> &&callback) {
            MyTodo tLib("");
            auto items = tLib.todo_list_elem();
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(items);
            callback(resp);
        }, {Get});
    app().registerHandler(
        "/api/todo/get/{name}",
        [](const HttpRequestPtr &,
           std::function<void(const HttpResponsePtr &)> &&callback,
           const std::string &name) {
            auto resp = HttpResponse::newHttpResponse();

            MyTodo tLib("");
            auto htm = tLib.dialog_show(stoi(name));
            resp->setBody(htm);
            callback(resp);
        },
        {Get});        
    app().registerHandler(
        "/api/todo/create", [](const HttpRequestPtr &request,  std::function<void(const HttpResponsePtr &)> &&callback) {
            auto json = request->getJsonObject();
            auto title = request->getParameter("title");
            Json::Value result;
            result["message"] = "user created";
            MyTodo tLib("");
            tLib.todo_add_handler(title);
            result["ret"] = "OK";
            auto items = tLib.todo_list_elem();
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(items);
            callback(resp);
        }, {Post});    
    app().registerHandler(
        "/api/todo/delete", [](const HttpRequestPtr &request,  std::function<void(const HttpResponsePtr &)> &&callback) {
            auto json = request->getJsonObject();
            auto id = request->getParameter("id");
            Json::Value result;
            result["message"] = "user created";
            MyTodo tLib("");
            tLib.todo_delete_handler(std::stoi(id));
            result["ret"] = "OK";
            auto items = tLib.todo_list_elem();
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(items);
            callback(resp);
        }, {Post});    

    //Load config file
    //drogon::app().loadConfigFile("../config.json");
    //drogon::app().loadConfigFile("../config.yaml");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
