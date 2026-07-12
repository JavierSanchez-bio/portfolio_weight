#ifndef PORTFOLIOSTORAGE_H
#define PORTFOLIOSTORAGE_H

#include <string>

class IdealPortfolio;
class ActualPortfolio;

class PortfolioStorage {
    public:
        explicit PortfolioStorage(std::string filename);

        bool Load(IdealPortfolio& ideal_portfolio, ActualPortfolio& actual_portfolio);
        bool Save(const IdealPortfolio& ideal_portfolio, const ActualPortfolio& actual_portfolio);

    private:
        std::string filename_;
};



#endif // PORTFOLIOSTORAGE_H