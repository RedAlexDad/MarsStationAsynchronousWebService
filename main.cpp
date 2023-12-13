#include "Asynchronous.h"
#include "json/json.h"
#include <microhttpd.h>
#include <cstring>



int main() {

    Asynchronous async;
    httplib::Server svr;

    // Для выполнения метода GET
    // std::string apiUrlGet = std::string(DOMEN_PYTHON) + "api/geographical_object/";
    // async.processJsonResponseGeographicalObject(apiUrlGet);

    // Для выполнения метода POST
    // std::string apiUrlPost = std::string(DOMEN_PYTHON) + "api/async_calc/";
    // // Создаем объект для POST-запроса
    // std::string data = R"({
    //     "feature": "Some feature",
    //     "type": "Some type",
    //     "size": 530,
    //     "describe": "Огромный низкий вулкан, расположенный в северной части региона Фарсида...",
    //     "photo": "",
    //     "status": false
    // })";
    // async.executePostRequest(apiUrlPost, data);

    // Обработка POST-запроса по указанному пути
    svr.Post("/api/async_calc/", [&async](const httplib::Request &req, httplib::Response &res) {
        async.POST_async_calc(req, res);
    });

    // svr.Put("/api/async_result/", [&async](const httplib::Request& req, httplib::Response& res) {
    //     async.PUT_async_result(req, res);
    // });

    // Для выполнения метода PUT
    // std::string apiUrlPut = std::string(DOMEN_PYTHON) + "api/async_result/";
    // // // Создаем объект для PUT-запроса
    // std::string data_put = R"({
    //     "id_draft": 1,
    //     "status_mission": "Успех",
    //     "access_token": ""
    // })";
    // async.executePutRequest(apiUrlPut, data_put);

    // Запуск сервера
    svr.listen(DOMEN_CPP, PORT);

    return 0;
}
