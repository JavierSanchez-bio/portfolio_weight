# portfolio_weight
  Full-stack app to allow the user to:  1- Register their protfolio (asset + quantity + currency)  2- Compute the value in the currency selected  3- Compute weight of each asset  4- Compare these weights with the ideal ones selected  5- Get warnings of over/under-weighted assets
  
  COSAS QUE ME SON ÚTILES:
  
  Comandos para correr la app:
  
  ./server
  
  
  Gestionar Database:
  http://0.0.0.0:8080


  COMPILE:
  g++ server.cpp PortfolioStorage.cpp MarketData.cpp -pthread -lssl -lcrypto -o server
