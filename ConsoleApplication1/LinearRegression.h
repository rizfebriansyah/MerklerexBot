#pragma once
#include "MerkelMain.h"


using namespace std;

class LinearRegression {
public:
	map<string, vector<double>*>* exchange_rate_database;
	void init_database(MerkelMain* app);
	map<string, double> Average_Asking(MerkelMain* app);
	void Store_Average_Asking(MerkelMain* app);
	double getMostRecentExchangeRate(string product);
	double getExchangeRateChange(string product, unsigned frames);
	double ExchangeRateLR(string product, unsigned frames);
	double ImpliedER(string productA, string productB);

	LinearRegression();
};