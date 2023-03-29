#include <iostream>
#include "DelimitedMessagesStreamParser.h"
#include <chrono>
#include <ctime>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>

using namespace std;


typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
typedef DelimitedMessagesStreamParser<WrapperMessage>::PointerToConstValue PointerToConstValue;


struct TestApi{
    static pair<PointerToConstData, WrapperMessage> getFastResponse(bool wrong);
    static pair<PointerToConstData, WrapperMessage> getSlowResponse(google::protobuf::uint32 connected_client_count = 10);
    static pair<PointerToConstData, WrapperMessage> getRequestForFastResponse();
    static pair<PointerToConstData, WrapperMessage> getRequestForSlowResponse(google::protobuf::uint32 time_in_seconds_to_sleep = 5);
};


void clear(vector<char> * buffer, size_t * bytesConsumed);
