#ifndef MARSSTATIONASYNCHRONOUSWEBSERVICE_ASYNCHRONOUS_H
#define MARSSTATIONASYNCHRONOUSWEBSERVICE_ASYNCHRONOUS_H

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include "httplib.h"

class Asynchronous {
public:
    Asynchronous();

    ~Asynchronous();

    // Метод для выполнения GET
    bool performGetCurlRequest(const std::string &url, std::string &response);

    // Метод для выполнения POST
    bool performPostCurlRequest(const std::string &url, const std::string &postData, std::string &response);

    // Метод для парсинга в JSON формата
    bool parseJsonResponse(const std::string &jsonResponse, Json::Value &parsedRoot);

    // Метод для выполнения запроса и вывода ответа
    void executeGetRequest(const std::string &apiUrl);

    // Метод для обработки JSON-ответа
    void processJsonResponseGeographicalObject(const std::string &apiUrl);

    // Метод для выполнения POST-запроса и вывода ответа
    void executePostRequestGeographicalObject(const std::string &apiUrl, const std::string &postData);

    // Метод для выполнения POST-запроса с postman
    void POST_async_calc(const httplib::Request &req, httplib::Response &res);

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
