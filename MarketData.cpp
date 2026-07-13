#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"
#include "MarketData.h"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

bool MarketData::LoadMappings(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open ticker mappings file: " << filename << "\n";
        return false;
    }

    try {
        json j;
        file >> j;
        
        for (auto& [key, value] : j.items()) {
            AssetData data;
            data.isin = value["isin"].get<std::string>();
            data.category = value["category"].get<std::string>();
            
            ticker_registry_[key] = data;
        }
        
        std::cout << "[INFO] Loaded " << ticker_registry_.size() << " assets with categories from " << filename << ".\n";
        return true;

    } catch (const json::exception& e) {
        std::cerr << "[ERROR] Failed to parse mappings JSON: " << e.what() << "\n";
        return false;
    }
}

double MarketData::GetPrice(const std::string& custom_ticker) {
    if (ticker_registry_.find(custom_ticker) == ticker_registry_.end()) {
        std::cerr << "[ERROR] No data found for custom ticker: " << custom_ticker << "\n";
        return 0.0;
    }
    
    std::string isin = ticker_registry_[custom_ticker].isin;

    std::string yahoo_ticker = GetYahooTickerFromISIN(isin);
    if (yahoo_ticker.empty()) return 0.0;

    return FetchPriceFromYahoo(yahoo_ticker);
}

std::string MarketData::GetCategory(const std::string& custom_ticker) {
    if (ticker_registry_.find(custom_ticker) != ticker_registry_.end()) {
        return ticker_registry_[custom_ticker].category;
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

    if (res && res->status == 200) {
        try {
            json j = json::parse(res->body);
            if (j.contains("quotes") && !j["quotes"].empty()) {
                return j["quotes"][0]["symbol"];
            }
        } catch (...) {
            std::cerr << "[ERROR] JSON parsing failed for ISIN: " << isin << "\n";
        }
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

    if (res && res->status == 200) {
        try {
            json j = json::parse(res->body);
            return j["chart"]["result"][0]["meta"]["regularMarketPrice"];
        } catch (...) {
            std::cerr << "[ERROR] Could not find price for Yahoo ticker: " << yahoo_ticker << "\n";
        }
    }
    return 0.0;
}