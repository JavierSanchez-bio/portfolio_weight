#include <iostream>
#include <string>
#include "IdealPortfolio.h"
#include "ActualPortfolio.h"
#include "httplib.h"

int main() {
    httplib::Server svr;
    IdealPortfolio ideal_portfolio;

    // Endpoint to update asset allocation
    svr.Post("/changeAsset", [&](const httplib::Request& req, httplib::Response& res) {
        
        // Validate payload
        if (req.has_param("ticker") && req.has_param("percentage")) {
            
            std::string ticker = req.get_param_value("ticker");
            double percentage = std::stod(req.get_param_value("percentage"));

            // Update in-memory portfolio
            ideal_portfolio.ChangeAsset(ticker, percentage);

            std::cout << "[INFO] Allocation updated: " << ticker << " set to " << percentage << "%.\n";
            
            // Success response
            res.status = 200;
            res.set_content("Asset successfully updated.", "text/plain");
        } else {
            // Handle missing parameters
            std::cerr << "[ERROR] Invalid request: Missing ticker or percentage parameters.\n";
            res.status = 400; 
            res.set_content("Bad Request: Missing parameters.", "text/plain");
        }
    });

    // Print ideal portfolio
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
        res.set_content(json, "application/json"); // Inform the browser it's receiving raw JSON data
    });

    // Health check endpoint
    bool mount_success = svr.set_mount_point("/", "./public");
    if (!mount_success) {
        std::cerr << "[ERROR] Could not find folder './public'. Make sure it exists.\n";
        return 1; 
    }

    // Start server loop
    std::cout << "[INFO] Market Data Server initializing...\n";
    std::cout << "[INFO] Listening on http://0.0.0.0:8080\n";
    svr.listen("0.0.0.0", 8080); 

    return 0;
}