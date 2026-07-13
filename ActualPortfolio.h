#ifndef ACTUALPORTFOLIO_H
#define ACTUALPORTFOLIO_H

#include <iostream>
#include <string>
#include <unordered_map>

class ActualPortfolio {
private:
    std::unordered_map<std::string, double> assets_;

public:
    ActualPortfolio() = default;

    bool ChangeAsset(const std::string& ticker, double units) {
        if (units <= 0.0) {
        assets_.erase(ticker);
        return false;
        } else {
            assets_[ticker] = units;
        }
        return true;
    }

    const std::unordered_map<std::string, double>& GetAssets() const {
        return assets_;
    }
};

#endif // ACTUALPORTFOLIO_H