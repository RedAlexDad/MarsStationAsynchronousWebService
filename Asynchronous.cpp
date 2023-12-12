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

bool Asynchronous::performCurlRequest(const std::string& url, std::string& response) {
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

bool Asynchronous::parseJsonResponse(const std::string& jsonResponse, Json::Value& parsedRoot) {
    std::string errs;
    bool parsingSuccessful = jsonReader->parse(jsonResponse.c_str(), jsonResponse.c_str() + jsonResponse.size(), &parsedRoot, &errs);
    if (!parsingSuccessful) {
        std::cerr << "Failed to parse JSON: " << errs << std::endl;
        return false;
    }

    return true;
}

void Asynchronous::executeRequest(const std::string& apiUrl) {
    std::string response;
    // Выполняем CURL запрос и выводим ответ
    if (performCurlRequest(apiUrl, response)) {
        std::cout << response << std::endl;
    } else {
        std::cerr << "Failed to perform CURL request. Error: " << curlError << std::endl;
    }
}

void Asynchronous::processJsonResponseGeographicalObject(const std::string& apiUrl) {
    std::string response;
    // Выполняем CURL запрос и выводим ответ
    if (performCurlRequest(apiUrl, response)) {
        // Теперь обработаем ответ
        Json::Value root;
        // Обработка каждого элемента результата
        if (parseJsonResponse(response, root)) {
            const Json::Value results = root["results"];
            for (const Json::Value &result : results) {
                std::cout << "Feature: " << result["feature"].asString() << std::endl;
                std::cout << "Type: " << result["Type"].asString() << std::endl;
                std::cout << "--------" << std::endl;
            }
        }
    } else {
        std::cerr << "Failed to perform CURL request. Error: " << curlError << std::endl;
    }
}