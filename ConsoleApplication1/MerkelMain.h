#pragma once
#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"


class MerkelMain
{
public:
    MerkelMain();
    /** Call this to start the sim */
    void init();
    void printMenu();
    void printHelp();
    void printMarketStats();
    void enterAsk();
    void enterBid();
    void printWallet();
    void gotoNextTimeframe();
    int getUserOption();
    void processUserOption(int userOption);

    std::string currentTime;
    OrderBook orderBook{ "C:/Users/user/Desktop/MerkelBotDelivery - 2/MerkelBot/ConsoleApplication1/20200317.csv" };

    Wallet wallet;

};
