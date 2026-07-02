#include <iostream>
#include <string>
#include "IdealPortfolio.h"
#include "ActualPortfolio.h"
#include "httplib.h"

int main() {
    httplib::Server svr;
    IdealPortfolio ideal_portfolio;
    ActualPortfolio actual_portfolio;

    // Ideal Portfolio Endpoints
    svr.Post("/changeAsset", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("ticker") && req.has_param("percentage")) {
            std::string ticker = req.get_param_value("ticker");
            double percentage = std::stod(req.get_param_value("percentage"));

            if (ideal_portfolio.ChangeAsset(ticker, percentage)) {
                std::cout << "[INFO] Target updated: " << ticker << " -> " << percentage << "%\n";
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
        std::string json = "[";
        bool first = true;
        for (const auto& [ticker, percentage] : ideal_portfolio.GetTargets()) {
            if (!first) json += ",";
            json += "{\"ticker\":\"" + ticker + "\",\"percentage\":" + std::to_string(percentage) + "}";
            first = false;
        }
        json += "]";
        res.status = 200;
        res.set_content(json, "application/json");
    });

    // Actual Portfolio Endpoints
    svr.Post("/changeActualAsset", [&](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("ticker") && req.has_param("money")) {
            std::string ticker = req.get_param_value("ticker");
            double money = std::stod(req.get_param_value("money"));

            if (actual_portfolio.ChangeAsset(ticker, money)) {
                std::cout << "[INFO] Balance updated: " << ticker << " -> $" << money << "\n";
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
        std::string json = "[";
        bool first = true;
        for (const auto& [ticker, money] : actual_portfolio.GetAssets()) {
            if (!first) json += ",";
            json += "{\"ticker\":\"" + ticker + "\",\"money\":" + std::to_string(money) + "}";
            first = false;
        }
        json += "]";
        res.status = 200;
        res.set_content(json, "application/json");
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