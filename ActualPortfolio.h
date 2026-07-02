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

    bool ChangeAsset(const std::string& ticker, double money) {
        if (money < 0.0) {
            std::cerr << "[ERROR] Invalid amount. Asset balance cannot be negative.\n";
            return false;
        }
        assets_[ticker] = money;
        return true;
    }

    const std::unordered_map<std::string, double>& GetAssets() const {
        return assets_;
    }
};

#endif // ACTUALPORTFOLIO_H