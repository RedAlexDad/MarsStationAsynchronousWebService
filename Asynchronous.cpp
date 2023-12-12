#include "Asynchronous.h"

Asynchronous::Asynchronous() {
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

void Asynchronous::executePostRequestGeographicalObject(const std::string &apiUrl, const std::string &postData) {
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

void Asynchronous::POST_async_calc(const httplib::Request &req, httplib::Response &res) {
    // Получаем тело запроса
    std::string body = req.body;

    // Пытаемся распарсить JSON из тела запроса
    Json::CharReaderBuilder jsonReader;
    Json::Value jsonParams;
    std::istringstream jsonStream(body);
    Json::parseFromStream(jsonReader, jsonStream, &jsonParams, nullptr);

    // Проверяем наличие параметра "id_draft"
    if (jsonParams.isMember("id_draft")) {
        // Параметр "id_draft" присутствует в JSON
        int id_draft = jsonParams["id_draft"].asInt();
        res.set_content("ID Draft: " + std::to_string(id_draft), "application/json");
    } else {
        // Параметр "id_draft" отсутствует в JSON
        res.set_content("Error: id_draft parameter is missing", "application/json");
    }
}