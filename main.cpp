#include "Asynchronous.h"

#define DOMEN "http://127.0.0.1:8000/"

int main() {

    Asynchronous async;

    // Для выполнения метода GET
    // std::string apiUrlGet = std::string(DOMEN) + "api/geographical_object/";
    // async.processJsonResponseGeographicalObject(apiUrlGet);

    // Для выполнения метода POST
    std::string apiUrlPost = std::string(DOMEN) + "api/async_calc/";
    // Создаем объект для POST-запроса
    std::string data = R"({
        "feature": "Some feature",
        "type": "Some type",
        "size": 530,
        "describe": "Огромный низкий вулкан, расположенный в северной части региона Фарсида...",
        "photo": "",
        "status": false
    })";
    async.executePostRequestGeographicalObject(apiUrlPost, data);

    return 0;
}
