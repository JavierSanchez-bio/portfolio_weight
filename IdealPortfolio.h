#ifndef IDEALPORTFOLIO_H
#define IDEALPORTFOLIO_H

#include <iostream>
#include <string>
#include <unordered_map>

class IdealPortfolio {
private:
    std::unordered_map<std::string, double> targets_;

    bool isValidPercentage(const std::string& ticker, double percentage) const {
        if (percentage < 0.0 || percentage > 100.0) return false;
        
        double projected_sum = 0.0;
        for (const auto& [key, value] : targets_) {
            if (key != ticker) {
                projected_sum += value;
            }
        }
        projected_sum += percentage;
        return projected_sum <= 100.0;
    }

public:
    IdealPortfolio() = default;

    bool ChangeAsset(const std::string& ticker, double percentage) {
        if (!isValidPercentage(ticker, percentage)) {
            std::cerr << "[ERROR] Allocation limit exceeded. Total portfolio cannot exceed 100%.\n";
            return false;
        }

        if (percentage == 0.0) {
            targets_.erase(ticker);
        } else {
            targets_[ticker] = percentage;
        }
        
        return true;
    }

    const std::unordered_map<std::string, double>& GetTargets() const {
        return targets_;
    }
};

#endif // IDEALPORTFOLIO_H