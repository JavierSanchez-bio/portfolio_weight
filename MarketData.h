#ifndef MARKETDATA_H
#define MARKETDATA_H

#include <string>
#include <unordered_map>

struct AssetData {
    std::string isin;
    std::string category;
};

class MarketData {
public:
    MarketData() = default;

    bool LoadMappings(const std::string& filename);
    double GetPrice(const std::string& custom_ticker);
    
    std::string GetCategory(const std::string& custom_ticker);

private:
    std::unordered_map<std::string, AssetData> ticker_registry_;

    std::string GetYahooTickerFromISIN(const std::string& isin);
    double FetchPriceFromYahoo(const std::string& yahoo_ticker);
};

#endif // MARKETDATA_H