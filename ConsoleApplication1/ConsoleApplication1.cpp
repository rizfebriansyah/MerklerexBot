#include <iostream>
#include <string>
#include <vector>
#include "OrderBookEntry.h"
#include "MerkelMain.h"
#include "CSVReader.h"
#include "Wallet.h"
#include "LinearRegression.h"

using namespace std;

int main()
{
    MerkelMain app{};
    app.init();

    /*Wallet wallet;
    wallet.insertCurrency("BTC", 10);
    wallet.insertCurrency("USDT", 10000);
    std::cout << wallet.toString() << std::endl;
    wallet.removeCurrency("USDT", 1000);
    std::cout << wallet.toString() << std::endl;*/

    LinearRegression LR;
    LR.init_database(&app);

    for (int i = 0; i < 3; i++) {
        LR.Store_Average_Asking(&app);

        cout << endl;
        cout << "BTC/USDT Linear Regression slope: " << LR.ExchangeRateLR("BTC/USDT", 100) << endl;
        cout << "BTC/USDT Last 10 frames Exchange rate: " << LR.getExchangeRateChange("BTC/USDT", 10) << endl;
        cout << "Current BTC/USDT: " << LR.getMostRecentExchangeRate("BTC/USDT") << endl;
        cout << "Current implied BTC/USDT with ETH: " << LR.ImpliedER("ETH/BTC", "ETH/USDT") << endl << endl;

        if (LR.ExchangeRateLR("BTC/USDT", 100) < 0) {
            app.enterAsk();
        }
        else {
            app.enterBid();
        }
        app.gotoNextTimeframe();
    }
    

    return 0;

}
