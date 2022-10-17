#include <iostream>
#include "LinearRegression.h"

using namespace std;


LinearRegression::LinearRegression() {}


/*
	Initializes the exchange rate database based on the given simulator's products
*/
void LinearRegression::init_database(MerkelMain* app) {
	this->exchange_rate_database = new map<string, vector<double>*>;
	vector<string> products = app->orderBook.getKnownProducts();
	for (string product : products) {
		this->exchange_rate_database->insert(pair<string, vector<double>*>(product, new vector<double>()));
	}
}


/*
	Returns a map of products to their average asking rates at the current time
	Ex. product_rate_map.at("ETH/USDT") holds the average price of ETH in USDT
*/
map<string, double> LinearRegression::Average_Asking(MerkelMain* app) {
	map<string, double> product_rate_map;

	vector<string> products = app->orderBook.getKnownProducts();
	for (string product : products) {

		double total_ask_price = 0;
		double total_ask_amount = 0;
		vector<OrderBookEntry> asks = app->orderBook.getOrders(OrderBookType::ask, product, app->currentTime);

		if (asks.size() < 1) {
			product_rate_map.insert(pair<string, double>(product, -1));
			continue;
		}

		for (OrderBookEntry ask : asks) {
			total_ask_price = total_ask_price + ask.price;
			total_ask_amount = total_ask_amount + ask.amount;
		}

		double average_ask_price = total_ask_price / asks.size();
		double average_ask_amount = total_ask_amount / asks.size();

		product_rate_map.insert(pair<string, double>(product, average_ask_price / average_ask_amount));
	}

	return product_rate_map;
}


/*
	Stores current average asking rates of all products in the exchange rate database
	Call this once per time frame to maintain the database for analysis
*/
void LinearRegression::Store_Average_Asking(MerkelMain* app) {
	map<string, double> product_rate_map = Average_Asking(app);

	vector<string> products = app->orderBook.getKnownProducts();
	for (string product : products) {
		if (product_rate_map.at(product) != -1)
			exchange_rate_database->at(product)->push_back(product_rate_map.at(product));
		else if (exchange_rate_database->at(product)->size() > 0)
			exchange_rate_database->at(product)->push_back(exchange_rate_database->at(product)->at(exchange_rate_database->at(product)->size() - 1));
	}
}


/*
	Returns most recently stored exchange rate of given product
*/
double LinearRegression::getMostRecentExchangeRate(string product) {
	if (exchange_rate_database->at(product)->size() > 0)
		return exchange_rate_database->at(product)->at(exchange_rate_database->at(product)->size() - 1);
	else
		return 0;
}


/*
	Returns the change in exchange rate for this product over the last (frames) amount of time frames

	Ex. get_average_exchange_rate_change("ETH/USDT", 10) returns a number indicating the change
		in the USDT to ETH exchange rate over the past 10 frames
*/
double LinearRegression::getExchangeRateChange(string product, unsigned frames) {

	vector<double>* product_exchange_rates = exchange_rate_database->at(product);

	if (product_exchange_rates->size() < 2)
		return 0;

	unsigned start_index;
	if (frames > product_exchange_rates->size())
		start_index = 0;
	else
		start_index = product_exchange_rates->size() - frames;

	unsigned end_index = product_exchange_rates->size() - 1;

	return product_exchange_rates->at(end_index) - product_exchange_rates->at(start_index);

}


/*
	Computes the linear regression of the given product's exchange rate over the last (frames) amount of time frames
	Returns the slope of the linear regression, representing the general upward or downward trend
*/
double LinearRegression::ExchangeRateLR(string product, unsigned frames) {
	vector<double>* product_exchange_rates = exchange_rate_database->at(product);

	if (product_exchange_rates->size() < 2)
		return 0;

	unsigned start_index;
	if (frames > product_exchange_rates->size())
		start_index = 0;
	else
		start_index = product_exchange_rates->size() - frames;

	unsigned end_index = product_exchange_rates->size() - 1;

	unsigned n = end_index - start_index + 1;

	double sum_rates_frames = 0;
	for (unsigned i = 0; i < n; i++) {
		sum_rates_frames += i * product_exchange_rates->at(start_index + i);
	}

	double sum_frames = 0;
	double sum_frames_sqr = 0;
	for (unsigned i = 0; i < n; i++) {
		sum_frames = sum_frames+ i;
		sum_frames_sqr = sum_frames_sqr+(double)i * i;
	}

	double sum_rates = 0;
	for (unsigned i = start_index; i < end_index + 1; i++) {
		sum_rates = sum_rates + product_exchange_rates->at(i);
	}

	return (n * sum_rates_frames - sum_frames * sum_rates) / (n * sum_frames_sqr - sum_frames * sum_frames);
}


/*
	Returns the impied exchange rate of two products based on a third party product

	Ex. get_implied_exchange_rate("ETH/BTC", "ETH/USDT") gives the implied rate of BTC/USDT
		If actual rate of BTC/USDT is higher, may want to use USDT to buy ETH,
		then ETH to buy BTC instead

	Ex. Suppose: BTC costs 40 USDT
				 BTC costs 2 ETH
				 ETH costs 15 USDT

		Instead of using 40 USDT to buy 1 BTC, use 30 USDT to buy 2 ETH, then 2 ETH to buy 1 BTC,
		ultimately saving 10 USDT.
*/
double LinearRegression::ImpliedER(string productA, string productB) {
	double productA_rate = getMostRecentExchangeRate(productA);
	double productB_rate = getMostRecentExchangeRate(productB);
	return productB_rate / productA_rate;
}