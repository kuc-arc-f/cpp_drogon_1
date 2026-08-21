#include <drogon/drogon.h>
#include "./include/my_ssr.hpp"

using namespace drogon;

int main() {
        // config.json を読み込む(リスンポート、DB接続情報など)
    drogon::app().loadConfigFile("config.json");

    // アプリ起動直前に todos テーブルが無ければ作成する
    drogon::app().registerBeginningAdvice([]() {
        auto dbClient = drogon::app().getDbClient();
        try
        {
            dbClient->execSqlSync(
                "CREATE TABLE IF NOT EXISTS todos ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "title TEXT NOT NULL, "
                "done INTEGER NOT NULL DEFAULT 0, "
                "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
                ")");
            LOG_INFO << "todos table is ready";
        }
        catch (const drogon::orm::DrogonDbException &e)
        {
            LOG_ERROR << "Failed to create todos table: " << e.base().what();
        }
    });

    app().registerHandler(
        "/", [](const HttpRequestPtr &request,  std::function<void(const HttpResponsePtr &)> &&callback) {
            MySsr sLib("");
            std::string htm = sLib.ssr_htm_top();
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody(htm);
            callback(resp);
        }, {Get});

    //Load config file
    //drogon::app().loadConfigFile("../config.json");
    //drogon::app().loadConfigFile("../config.yaml");
    //Run HTTP framework,the method will block in the internal event loop
    LOG_INFO << "Server starting on 0.0.0.0:8080";
    drogon::app().run();
    return 0;
}
