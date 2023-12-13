#include <future>
#include "Asynchronous.h"

Asynchronous::Asynchronous() {
    // Инициализация генератора случайных чисел с использованием текущего времени
    std::srand(std::time(0));

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    jsonReader = jsonBuilder.newCharReader();
}

Asynchronous::~Asynchronous() {
    curl_easy_cleanup(curl);
    delete jsonReader;
}

size_t Asynchronous::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *) userp)->append((char *) contents, size * nmemb);
    return size * nmemb;
}

bool Asynchronous::performGetCurlRequest(const std::string &url, std::string &response) {
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curlError = curl_easy_strerror(res);
        return false;
    }

    return true;
}

bool Asynchronous::performPostCurlRequest(const std::string &url, const std::string &postData, std::string &response) {
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return false;
    }

    // Устанавливаем URL для POST-запроса
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Включаем метод POST
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // Устанавливаем данные для POST-запроса
    curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, postData.c_str());

    // Устанавливаем функцию обратного вызова для записи ответа
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curlError = curl_easy_strerror(res);
        return false;
    }

    // Получаем код HTTP-ответа
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (httpCode != 200) {
        std::cerr << "HTTP request failed with code: " << httpCode << std::endl;
        return false;
    }

    return true;
}

bool Asynchronous::performPutCurlRequest(const std::string &url, const std::string &putData, std::string &response) {
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return false;
    }

    // Устанавливаем URL для PUT-запроса
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Включаем метод PUT
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    // Устанавливаем данные для PUT-запроса
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, putData.c_str());

    // Устанавливаем функцию обратного вызова для записи ответа
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "CURL request failed with error: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    // Получаем код HTTP-ответа
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (httpCode != 200) {
        std::cerr << "HTTP request failed with code: " << httpCode << std::endl;
        return false;
    }

    return true;
}


bool Asynchronous::parseJsonResponse(const std::string &jsonResponse, Json::Value &parsedRoot) {
    std::string errs;
    bool parsingSuccessful = jsonReader->parse(jsonResponse.c_str(), jsonResponse.c_str() + jsonResponse.size(),
                                               &parsedRoot, &errs);
    if (!parsingSuccessful) {
        std::cerr << "Failed to parse JSON: " << errs << std::endl;
        return false;
    }

    return true;
}

void Asynchronous::executeGetRequest(const std::string &apiUrl) {
    std::string response;
    // Выполняем CURL запрос и выводим ответ
    if (performGetCurlRequest(apiUrl, response)) {
        std::cout << response << std::endl;
    } else {
        std::cerr << "Failed to perform CURL request. Error: " << curlError << std::endl;
    }
}

void Asynchronous::processJsonResponseGeographicalObject(const std::string &apiUrl) {
    std::string response;
    // Выполняем CURL запрос и выводим ответ
    if (performGetCurlRequest(apiUrl, response)) {
        // Теперь обработаем ответ
        Json::Value root;
        // Обработка каждого элемента результата
        if (parseJsonResponse(response, root)) {
            const Json::Value results = root["results"];
            for (const Json::Value &result: results) {
                std::cout << "Feature: " << result["feature"].asString() << std::endl;
                std::cout << "Type: " << result["Type"].asString() << std::endl;
                std::cout << "--------" << std::endl;
            }
        }
    } else {
        std::cerr << "Failed to perform CURL request. Error: " << curlError << std::endl;
    }
}

void Asynchronous::executePostRequest(const std::string &apiUrl, const std::string &postData) {
    std::string response;
    // Выполняем POST-запрос и выводим ответ
    if (performPostCurlRequest(apiUrl, postData, response)) {
        std::cout << response << std::endl;
        // Теперь можно обработать ответ с использованием нового метода
        Json::Value root;
        if (parseJsonResponse(response, root)) {
            processJsonResponseGeographicalObject(response);
        }
    } else {
        std::cerr << "Failed to perform CURL request. Error: " << curlError << std::endl;
    }
}

void Asynchronous::executePutRequestAsync(const std::string &apiUrl, const std::string &putData) {
    std::string response;
    // Выполняем PUT-запрос и выводим ответ
    if (performPutCurlRequest(apiUrl, putData, response)) {
        std::cout << "response: " << response << std::endl;
        // Преобразуем строку JSON в объект Json::Value
        Json::Value jsonResponse;
        std::istringstream jsonStream(response);
        jsonStream >> jsonResponse;

        // Извлекаем значение "data"
        Json::Value data = jsonResponse["data"];

        // Извлекаем нужные значения из "data"
        std::cout << "--------------" << std::endl;
        std::cout << "id: " << data["id"].asInt() << std::endl;
        // std::cout << "type_status: " << data["type_status"].asString() << std::endl;
        // std::cout << "date_create: " << data["date_create"].asString() << std::endl;
        // std::cout << "date_form: " << data["date_form"].asString() << std::endl;
        // std::cout << "date_close: " << data["date_close"].asString() << std::endl;
        std::cout << "status_task: " << data["status_task"].asString() << std::endl;
        std::cout << "status_mission: " << data["status_mission"].asString() << std::endl;
        // std::cout << "id_employee: " << data["id_employee"].asString() << std::endl;
        // std::cout << "id_moderator: " << data["id_moderator"].asString() << std::endl;
        // std::cout << "id_transport: " << data["id_transport"].asString() << std::endl;
        std::cout << "--------------" << std::endl;
    } else {
        std::cerr << "Failed to perform CURL request. Error: " << curlError << std::endl;
    }
}

// Вспомогательная функция для разбора данных формы
void Asynchronous::parseFormData(const std::string &body, std::unordered_map<std::string, std::string> &formData) {
    std::istringstream stream(body);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        size_t equalsPos = pair.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = pair.substr(0, equalsPos);
            std::string value = pair.substr(equalsPos + 1);
            formData[key] = value;
        }
    }
}

void Asynchronous::POST_async_calc(const httplib::Request &req, httplib::Response &res) {
    // Получаем тело запроса
    std::string body = req.body;

    // Разбираем данные формы
    std::unordered_map<std::string, std::string> formData;
    parseFormData(body, formData);

    // Проверяем наличие параметра "id_draft"
    if (formData.find("id_draft") != formData.end()) {
        // Параметр "id_draft" присутствует в данных формы
        int id_draft = std::stoi(formData["id_draft"]);
        std::string access_token = formData["access_token"];

        res.set_content("id_draft: " + std::to_string(id_draft), "application/json");
        res.set_content("access_token: " + access_token, "application/json");

        // Запускаем асинхронную задачу через 2 секунд
        std::thread([this, id_draft, access_token]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            callNextMethod(id_draft, access_token);
        }).detach();
    } else {
        // Параметр "id_draft" отсутствует в данных формы
        res.set_content("Error: id_draft parameter is missing", "application/json");
    }
}

void Asynchronous::callNextMethod(int id_draft, const std::string &access_token) {
    // Создаем JSON-объект с данными для следующего метода
    nlohmann::json jsonData;
    jsonData["id_draft"] = id_draft;
    jsonData["status_mission"] = generateRandomResult();
    jsonData["access_token"] = access_token;

    // Преобразуем JSON в строку
    std::string jsonString = jsonData.dump();
    // std::cout << jsonString << std::endl;

    // Создаем promise и future для управления выполнением задачи
    std::promise<void> promise;
    std::future<void> future = promise.get_future();

    // Запускаем асинхронную задачу через 3 секунд
    std::thread([this, jsonString, &promise]() {
        Asynchronous async;
        std::string apiUrlPut = std::string(DOMEN_PYTHON) + "api/async_result/";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        async.executePutRequestAsync(apiUrlPut, jsonString);
    }).detach();

    // Ожидаем завершения задачи (не блокируем основной поток)
    future.wait();
}


void Asynchronous::PUT_async_result(const httplib::Request &req, httplib::Response &res) {
    // Получаем тело запроса
    std::string body = req.body;

    // Пытаемся распарсить JSON из тела запроса
    Json::CharReaderBuilder jsonReader;
    Json::Value jsonParams;
    std::istringstream jsonStream(body);
    Json::parseFromStream(jsonReader, jsonStream, &jsonParams, nullptr);

    // Проверяем наличие параметра "id_draft" и "status_mission"
    if (jsonParams.isMember("id_draft") && jsonParams.isMember("status_mission")) {
        // Параметры присутствуют в JSON
        int id_draft = jsonParams["id_draft"].asInt();
        std::string status_mission = jsonParams["status_mission"].asString();

        // Дополнительная обработка, если есть еще параметры

        res.set_content("ID Draft: " + std::to_string(id_draft) + "\nStatus Mission: " + status_mission,
                        "application/json");
    } else {
        // Параметры отсутствуют в JSON
        res.set_content("Error: id_draft or status_mission parameter is missing", "application/json");
    }
}

int Asynchronous::generateRandomResult() {
    // Генерация случайного числа от 1 до 100
    int randomNumber = std::rand() % 100 + 1;
    // Условие для выбора 2 для "УСПЕХ" или 3 для "ПОТЕРЯ" в соотношении 70:30
    return (randomNumber <= 70) ? 2 : 3;
}