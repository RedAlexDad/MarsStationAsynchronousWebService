#ifndef MARSSTATIONASYNCHRONOUSWEBSERVICE_ASYNCHRONOUS_H
#define MARSSTATIONASYNCHRONOUSWEBSERVICE_ASYNCHRONOUS_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>

#define DOMEN "http://127.0.0.1:8000/"

class Asynchronous {
public:
    Asynchronous();
    ~Asynchronous();

    // Метод для выполнения REST-API
    bool performCurlRequest(const std::string& url, std::string& response);
    // Метод для парсинга в JSON формата
    bool parseJsonResponse(const std::string& jsonResponse, Json::Value& parsedRoot);
    // Метод для выполнения запроса и вывода ответа
    void executeRequest(const std::string& apiUrl);
    // Метод для обработки JSON-ответа
    void processJsonResponseGeographicalObject(const std::string& apiUrl);
private:
    // Callback функция для обработки ответа CURL
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

    // Приватные члены для работы с CURL
    CURL *curl;
    std::string curlError;

    // Приватные члены для работы с JSON
    Json::CharReaderBuilder jsonBuilder;
    Json::CharReader *jsonReader;
};

#endif //MARSSTATIONASYNCHRONOUSWEBSERVICE_ASYNCHRONOUS_H
