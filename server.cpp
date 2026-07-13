#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include <string>
#include "IdealPortfolio.h"
#include "ActualPortfolio.h"
#include "PortfolioStorage.h"
#include "httplib.h"
#include "json.hpp"
#include "MarketData.h"


int main() {
    httplib::Server svr;
    IdealPortfolio ideal_portfolio;
    ActualPortfolio actual_portfolio;
    PortfolioStorage storage("portfolio_db.json");


    if (!storage.Load(ideal_portfolio, actual_portfolio)) {
        std::cerr << "[WARN] Failed to load persistent data. Starting fresh.\n";
    }

    MarketData market;
    if (!market.LoadMappings("ISIN.json")) {
        std::cerr << "Warning: Starting without predefined ticker mappings.\n";
    }

    svr.Get("/api/mappings", [](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file("ISIN.json");
        if (!file.is_open()) {
            res.status = 404;
            res.set_content("{\"error\": \"Mappings file not found\"}", "application/json");
            return;
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        res.set_content(content, "application/json");
    });

    svr.Get("/api/price", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("ticker")) {
            std::string ticker = req.get_param_value("ticker");
            double price = market.GetPrice(ticker); 
            
            nlohmann::json j_res;
            j_res["price"] = price;
            res.set_content(j_res.dump(), "application/json");
        } else {
            res.status = 400;
            res.set_content("{\"error\": \"Missing ticker param\"}", "application/json");
        }
    });

    // Ideal Portfolio Endpoints
    svr.Post("/changeAsset", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("ticker") && req.has_param("percentage")) {
            std::string ticker = req.get_param_value("ticker");
            double percentage = std::stod(req.get_param_value("percentage"));

            if (ideal_portfolio.ChangeAsset(ticker, percentage)) {
                std::cout << "[INFO] Target updated: " << ticker << " -> " << percentage << "%\n";
                storage.Save(ideal_portfolio, actual_portfolio);
                res.status = 200;
                res.set_content("Target allocation updated successfully.", "text/plain");
            } else {
                res.status = 400;
                res.set_content("Error: Total allocation cannot exceed 100%.", "text/plain");
            }
        } else {
            res.status = 400;
            res.set_content("Bad Request: Missing parameters.", "text/plain");
        }
    });

    svr.Get("/getPortfolio", [&](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json j_arr = nlohmann::json::array();
        for (const auto& [ticker, percentage] : ideal_portfolio.GetTargets()) {
            nlohmann::json j_asset;
            j_asset["ticker"] = ticker;
            j_asset["percentage"] = percentage;
            j_arr.push_back(j_asset);
        }
        res.status = 200;
        res.set_content(j_arr.dump(), "application/json");
    });

    // Actual Portfolio Endpoints
    svr.Post("/changeActualAsset", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("ticker") && req.has_param("units")) {
            std::string ticker = req.get_param_value("ticker");
            double units = std::stod(req.get_param_value("units"));

            if (actual_portfolio.ChangeAsset(ticker, units)) {
                std::cout << "[INFO] Balance updated: " << ticker << " :"<< units << "\n";
                storage.Save(ideal_portfolio, actual_portfolio);
                res.status = 200;
                res.set_content("Asset balance updated successfully.", "text/plain");
            } else {
                res.status = 400;
                res.set_content("Error: Asset balance cannot be negative.", "text/plain");
            }
        } else {
            res.status = 400;
            res.set_content("Bad Request: Missing parameters.", "text/plain");
        }
    });

    svr.Get("/getActualPortfolio", [&](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json j_arr = nlohmann::json::array();
        for (const auto& [ticker, units] : actual_portfolio.GetAssets()) {
            double price = market.GetPrice(ticker);
            double total_value = units * price; 

            nlohmann::json j_asset;
            j_asset["ticker"] = ticker;
            j_asset["units"] = units;
            j_asset["price"] = price;
            j_asset["value"] = total_value;
            j_arr.push_back(j_asset);
        }
        res.status = 200;
        res.set_content(j_arr.dump(), "application/json");
    });


    // Static Web File Serving
    if (!svr.set_mount_point("/", "./public")) {
        std::cerr << "[ERROR] Static root directory './public' not found.\n";
        return 1; 
    }

    std::cout << "[INFO] Server initializing...\n";
    std::cout << "[INFO] Listening on http://0.0.0.0:8080\n";
    svr.listen("0.0.0.0", 8080); 

    return 0;
}