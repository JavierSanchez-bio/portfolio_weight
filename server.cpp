#include <iostream>
#include <map>
#include <string>
#include "httplib.h"

int main() {
    httplib::Server svr;
    IdealPortfolio mi_portfolio;

    // 1. Definimos el "Endpoint" que escuchará al Frontend para cambiar activos
    // Cuando desde el navegador o app pulses "changeAsset", llegará aquí
    svr.Post("/changeAsset", [&](const httplib::Request& req, httplib::Response& res) {
        // Aquí recuperarás los datos enviados por el botón del Frontend
        // Por ejemplo, asumiendo que llegan las variables "ticker" y "porcentaje"
        std::string ticker = req.get_file_value("ticker").content;
        double porcentaje = std::stod(req.get_file_value("percentage").content);

        // Modificamos el portfolio en memoria de forma instantánea
        mi_portfolio.add_or_modify(ticker, porcentaje);

        std::cout << "Frontend ordenó modificar: " << ticker << " al " << porcentaje << "%\n";
        
        // Respondemos al Frontend que todo ha ido bien
        res.set_content("¡Activo actualizado!", "text/plain");
    });

    // 2. Arrancamos el servidor en el puerto 8080 de tu Ubuntu
    // Este método contiene el bucle infinito eficiente que se queda esperando peticiones
    std::cout << "Servidor de Market Data escuchando en http://localhost:8080...\n";
    svr.listen("0.0.0.0", 8080); 

    return 0;
}