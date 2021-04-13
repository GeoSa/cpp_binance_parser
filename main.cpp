#include <iostream>
#include <json/json.h>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

#include "binance.h"
#include "binance_logger.h"
#include "binance_websocket.h"

using namespace binance;
using namespace std;

map<long, map<string, double> > priceCache;

static void print_priceCache()
{
	map < long, map<string,double> >::iterator it_i;
	char buffer[80];
	double price = 0;

	ofstream outfile("BTCUSD.log", ios::app);
	
	for ( it_i = priceCache.begin() ; it_i != priceCache.end() ; it_i++ ) 
	{

		long timestamp = (*it_i).first;
		map <string, double> price_obj = (*it_i).second;

		time_t rawtime = (time_t) timestamp;
		struct tm * timeinfo;
		char buffer [80];                                

		time ( &rawtime );                               
		timeinfo = localtime ( &rawtime );               
		strftime (buffer,80,"%Y/%m/%d %X ",timeinfo); 
		
		if (price != price_obj["c"])
		{
			price = price_obj["c"];

			outfile << buffer << " " << "BTCUSD " << price << "\n";
		}
		else
			continue;
			
	}
}

static int ws_prices_onData(Json::Value& json_result)
{	
	long timestamp = json_result["E"].asInt64();
	priceCache[timestamp]["c"] = atof( json_result["c"].asString().c_str());
	
	print_priceCache();
	
	return 0;
}

int main()
{
	Logger::set_debug_level(1);
	Logger::set_debug_logfp(stderr);

	Json::Value result;

	Server server;
	
	Market market(server);
		
 	Websocket::init();
 	Websocket::connect_endpoint(ws_prices_onData, "/ws/btcusdt@ticker"); 
	Websocket::enter_event_loop();
	
	return 0;
}

