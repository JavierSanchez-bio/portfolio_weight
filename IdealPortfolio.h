#ifndef IDEALPORTFOLIO_H
#define IDEALPORTFOLIO_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>


class IdealPortfolio {
private:
	std::unordered_map<std::string, double> targets_;

public:
	IdealPortfolio(const std::unordered_map<std::string, double>& targets) : targets_(targets) {}
	IdealPortfolio() = default;
    
    void ChangeAsset(const std::string& ticker, double percentage) {
        targets_[ticker] = percentage;
    }
    
};


#endif // IDEALPORTFOLIO_H