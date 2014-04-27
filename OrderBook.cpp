#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<list>
#include<map>
#include<algorithm>
using namespace std;

class OrderBook
{
private:
    struct OrderRec
	{
        unsigned int time;
        unsigned int orderId;
        double orderPrice;
        char orderType;
    };
    OrderRec _orderRec;
    list<OrderRec> _orderList;
public:
   OrderBook()
   { 
   }
   void insert(unsigned int time,unsigned int orderId, double price)
   {
        _orderRec.time = time;
        _orderRec.orderId = orderId;
        _orderRec.orderPrice = price;
        _orderRec.orderType = 'I';
        _orderList.push_back(_orderRec);
    }
    void erase(unsigned int time, unsigned int orderId)
    {
        _orderRec.time = time;
        _orderRec.orderId = orderId;
        _orderRec.orderType = 'E';
        _orderRec.orderPrice = 0;
        _orderList.push_back(_orderRec);
    }
    double getTimeWeightedAvgPrice()
    {

        double avgPrice = 0;
        double maxPrice = 0;
        unsigned int endTime=0,beginTime=0;
        typedef multimap<double,OrderRec,greater<double> > MapOfOrders;
        
	MapOfOrders ordersMap;
        list<OrderRec>::iterator listItr = _orderList.begin();
        for(;listItr!=_orderList.end();listItr++)
        {
            OrderRec listRec = *listItr;
            if(ordersMap.empty())
            {
                ordersMap.insert(make_pair(listRec.orderPrice,listRec));
                if(endTime!=0) //no orders in the orderbook scenario. Deduct this time period
                {
                    
                    beginTime += (listRec.time - endTime);
                }
                else
                {
                    beginTime = listRec.time;
                }
            }
            else if(listRec.orderType == 'I')
            {
                MapOfOrders::iterator mapItr = ordersMap.begin();
                OrderRec mapRec = (*mapItr).second;
                maxPrice = mapRec.orderPrice;
                if(listRec.orderPrice >= maxPrice)
                {
                    if(listRec.orderPrice == maxPrice)
                    {
                        listRec.time = mapRec.time;
                    }
                    else
                    {
                        avgPrice = avgPrice + ((listRec.time - mapRec.time) * maxPrice);
                    }
                    ordersMap.insert(make_pair(listRec.orderPrice,listRec));
                }
                else{
                    ordersMap.insert(make_pair(listRec.orderPrice,listRec));
                }
            }
            else if (listRec.orderType == 'E')
            {
                auto mapItr1 = find_if(ordersMap.begin(),ordersMap.end(),
                                [&](const MapOfOrders::value_type& val)
                                { 
                                    OrderRec r = val.second;
                                    return (r.orderId == listRec.orderId);
                                });
                if(mapItr1 != ordersMap.end())
                {
                    OrderRec r1 = (*mapItr1).second;
                    ordersMap.erase(mapItr1);
                    auto mapItr2 = ordersMap.begin();
                    if((mapItr2 != ordersMap.end())&& (r1.orderPrice > ((*mapItr2).second).orderPrice))
                    {
                        avgPrice += (listRec.time - r1.time)*r1.orderPrice;
                        ((*mapItr2).second).time = listRec.time;
                    }
                    else if(mapItr2 == ordersMap.end())
                    {
                        avgPrice += (listRec.time - r1.time)*r1.orderPrice;
                        endTime = listRec.time;
                    }
                }
            }
        }
        if(endTime == beginTime)
	{
		//throw an exception
		// OR return NAN;
	}
	else
	{
		return avgPrice/(endTime-beginTime);
	}
    }
};
int main(int argc, char *argv[])
{
    OrderBook ob;
    ifstream in("C:/input.txt");
    string orderRecord;
    unsigned int time, orderId;;
    char orderType;
    double orderPrice;
    while(getline(in,orderRecord))
    {
        istringstream iss(orderRecord);
        iss >> time;
        iss >> orderType;
        if(orderType == 'I')
        {
            iss >> orderId;
            iss >> orderPrice;
            ob.insert(time,orderId,orderPrice);
        }
        else if(orderType == 'E')
        {
            iss >> orderId;
            ob.erase(time,orderId);
        }
        else {
            cerr << "Invalid type" << orderType << "encountered\n";
            return -1;
        }
    }
    try{
        cout << "time weighted average price is " << ob.getTimeWeightedAvgPrice() << endl;
    }catch(...){
        cout << "Exception in getTimedWeightedAvgPrice\n";
    }
    return 0;
}