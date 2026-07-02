#ifndef ACTUALPORTFOLIO_H
#define ACTUALPORTFOLIO_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>


class ActualPortfolio {
private:
	std::unordered_map<std::string, double> assets_;

public:
	ActualPortfolio(const std::unordered_map<std::string, double>& assets) : assets_(assets) {}
	ActualPortfolio() = default;
    
    void ChangeAsset(const std::string& ticker, double money) {
        assets_[ticker] = money;
    }

    // Add this inside the public: section of ActualPortfolio.h
    const std::unordered_map<std::string, double>& GetAssets() const {
        return assets_;
    }
    
};

#endif // ACTUALPORTFOLIO_H