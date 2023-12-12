//
// Created by redalexdad on 12.12.23.
//

#include "Asynchronous.h"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>

using namespace std;

// Callback function to handle curl's response
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to perform CURL request
bool performCurlRequest(const string& url, string& response) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL" << endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        return false;
    }

    return true;
}

// Function to parse JSON response
bool parseJsonResponse(const string& jsonResponse, Json::Value& parsedRoot) {
    Json::CharReaderBuilder builder;
    Json::CharReader *reader = builder.newCharReader();
    string errs;

    bool parsingSuccessful = reader->parse(jsonResponse.c_str(), jsonResponse.c_str() + jsonResponse.size(), &parsedRoot, &errs);
    delete reader;

    if (!parsingSuccessful) {
        cerr << "Failed to parse JSON: " << errs << endl;
        return false;
    }

    return true;
}

int main() {
    string api_url = "https://marketdata.tradermade.com/api/v1/live?currency=EURUSD,GBPUSD,UK100&api_key=TVNEwwNmujceFEHrpVZg";
    string response;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    if (performCurlRequest(api_url, response)) {
        Json::Value root;
        if (parseJsonResponse(response, root)) {
            const Json::Value quotes = root["quotes"];
            for (const Json::Value &quote : quotes) {
                if (quote.isMember("bid") && quote.isMember("ask")) {
                    double bid = quote["bid"].asDouble();
                    double ask = quote["ask"].asDouble();
                    cout << "Bid: " << bid << ", Ask: " << ask << endl;
                }
            }
        }
    }

    curl_global_cleanup();
    return 0;
}