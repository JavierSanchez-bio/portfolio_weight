#include "PortfolioStorage.h"
#include "IdealPortfolio.h"
#include "ActualPortfolio.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>

PortfolioStorage::PortfolioStorage(std::string filename) : filename_(std::move(filename)) {}

bool PortfolioStorage::Load(IdealPortfolio& ideal_portfolio, ActualPortfolio& actual_portfolio) {
    std::ifstream file(filename_);
    if (!file.is_open()) {
        std::cout << "[INFO] No existing storage file found. Starting fresh.\n";
        return true;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string type, ticker, value_str;
        if (std::getline(ss, type, ',') && 
            std::getline(ss, ticker, ',') && 
            std::getline(ss, value_str, ',')) {
            
            try {
                double value = std::stod(value_str);
                if (type == "I") {
                    ideal_portfolio.ChangeAsset(ticker, value);
                } else if (type == "A") {
                    actual_portfolio.ChangeAsset(ticker, value);
                }
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Corruption detected while parsing line: " << line << "\n";
                return false;
            }
        }
    }

    file.close();
    std::cout << "[INFO] Portfolio data loaded successfully from disk.\n";
    return true;
}

bool PortfolioStorage::Save(const IdealPortfolio& ideal_portfolio, const ActualPortfolio& actual_portfolio) {
    std::ofstream file(filename_, std::ios::trunc); // Overwrites the old file
    
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open storage file for writing.\n";
        return false;
    }

    // Persist Ideal Portfolio targets (prefixed with 'I')
    for (const auto& [ticker, percentage] : ideal_portfolio.GetTargets()) {
        file << "I," << ticker << "," << percentage << "\n";
    }

    // Persist Actual Portfolio assets (prefixed with 'A')
    for (const auto& [ticker, money] : actual_portfolio.GetAssets()) {
        file << "A," << ticker << "," << money << "\n";
    }

    file.close();
    std::cout << "[INFO] Portfolio data persisted to disk.\n";
    return true;
}