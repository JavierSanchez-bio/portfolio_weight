#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"
#include "MarketData.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

using json = nlohmann::json;

bool MarketData::LoadMappings(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    try {
        json j;
        file >> j;
        
        ticker_registry_.clear();
        for (auto& [key, value] : j.items()) {
            AssetData data;
            data.isin = value["isin"].get<std::string>();
            data.category = value["category"].get<std::string>();
            
            std::string upper_key = key;
            std::transform(upper_key.begin(), upper_key.end(), upper_key.begin(), [](unsigned char c) {
                return std::toupper(c);
            });
            
            ticker_registry_[upper_key] = data;
        }
        return true;
    } catch (const json::exception& e) {
        return false;
    }
}

double MarketData::GetPrice(const std::string& custom_ticker) {
    std::string upper_ticker = custom_ticker;
    std::transform(upper_ticker.begin(), upper_ticker.end(), upper_ticker.begin(), [](unsigned char c) {
        return std::toupper(c);
    });

    if (ticker_registry_.find(upper_ticker) == ticker_registry_.end()) {
        return FetchPriceFromYahoo(upper_ticker);
    }
    
    std::string isin = ticker_registry_[upper_ticker].isin;
    
    std::string yahoo_ticker = GetYahooTickerFromISIN(isin);
    
    if (yahoo_ticker.empty()) {
        return FetchPriceFromYahoo(upper_ticker);
    }

    return FetchPriceFromYahoo(yahoo_ticker);
}

std::string MarketData::GetCategory(const std::string& custom_ticker) {
    std::string upper_ticker = custom_ticker;
    std::transform(upper_ticker.begin(), upper_ticker.end(), upper_ticker.begin(), [](unsigned char c) {
        return std::toupper(c);
    });

    if (ticker_registry_.find(upper_ticker) != ticker_registry_.end()) {
        return ticker_registry_[upper_ticker].category;
    }
    return "Unknown";
}

std::string MarketData::GetYahooTickerFromISIN(const std::string& isin) {
    httplib::SSLClient cli("query2.finance.yahoo.com");
    
    httplib::Headers headers = {
        {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"}
    };

    std::string path = "/v1/finance/search?q=" + isin;
    auto res = cli.Get(path.c_str(), headers);

    if (!res) {
        return "";
    }

    if (res->status == 200) {
        try {
            json j = json::parse(res->body);
            if (j.contains("quotes") && !j["quotes"].empty()) {
                std::string symbol = j["quotes"][0]["symbol"];
                return symbol;
            }
        } catch (...) {}
    }
    return "";
}

double MarketData::FetchPriceFromYahoo(const std::string& yahoo_ticker) {
    if (yahoo_ticker.empty()) return 0.0;

    httplib::SSLClient cli("query1.finance.yahoo.com");

    httplib::Headers headers = {
        {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"}
    };

    std::string path = "/v8/finance/chart/" + yahoo_ticker;
    auto res = cli.Get(path.c_str(), headers);

    if (!res) {
        return 0.0;
    }

    if (res->status == 200) {
        try {
            json j = json::parse(res->body);
            double price = j["chart"]["result"][0]["meta"]["regularMarketPrice"];
            return price;
        } catch (...) {}
    }
    return 0.0;
}