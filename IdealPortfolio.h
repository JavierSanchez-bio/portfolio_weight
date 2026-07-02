#ifndef IDEALPORTFOLIO_H
#define IDEALPORTFOLIO_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

class IdealPortfolio {
private:
	std::unordered_map<std::string, double> targets_;

    // Ahora calcula la suma PROYECTADA sustituyendo el valor si el activo ya existe
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
	IdealPortfolio(const std::unordered_map<std::string, double>& targets) : targets_(targets) {}
	IdealPortfolio() = default;
    
    // Cambiamos a bool para comunicar el resultado al servidor
    bool ChangeAsset(const std::string& ticker, double percentage) {
        if (!isValidPercentage(ticker, percentage)) {
            std::cerr << "[ERROR] Invalid target allocation. Total portfolio cannot exceed 100%.\n";
            return false;
        }
        targets_[ticker] = percentage;
        return true;
    }

    const std::unordered_map<std::string, double>& GetTargets() const {
        return targets_;
    }
};

#endif // IDEALPORTFOLIO_H