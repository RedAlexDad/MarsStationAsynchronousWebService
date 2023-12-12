#include "Asynchronous.h"
#include "json/json.h"
#include <microhttpd.h>
#include <cstring>

#define DOMEN_PYTHON "http://127.0.0.1:8000/"
#define PORT 5000
#define DOMEN_CPP "127.0.0.1"

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
    // async.executePostRequestGeographicalObject(apiUrlPost, data);

    // Обработка POST-запроса по указанному пути
    svr.Post("/api/async_calc/", [&async](const httplib::Request &req, httplib::Response &res) {
        async.POST_async_calc(req, res);
    });

    // Запуск сервера
    svr.listen(DOMEN_CPP, PORT);

    return 0;
}
