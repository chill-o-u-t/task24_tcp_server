#include <iostream>
#include "DelimitedMessagesStreamParser.h"
#include <chrono>
#include <ctime>
#include <test_api.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <memory>
#include <vector>

using namespace std;


typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
typedef DelimitedMessagesStreamParser<WrapperMessage>::PointerToConstValue PointerToConstValue;


int main() {
    vector<char> messages;
    pair<PointerToConstData, WrapperMessage> fast_response = TestApi::getFastResponse(false);
    pair<PointerToConstData, WrapperMessage> request_for_fast_response = TestApi::getRequestForFastResponse();
    pair<PointerToConstData, WrapperMessage> slow_response = TestApi::getSlowResponse(322);
    pair<PointerToConstData, WrapperMessage> request_for_slow_response = TestApi::getRequestForSlowResponse(1300);

    string str = "ksdkbfdskakjdfsdf";

    messages.insert(messages.end(), str.begin(), str.end());
    messages.insert(messages.end(), request_for_fast_response.first->begin(), request_for_fast_response.first->end());
    messages.insert(messages.end(), request_for_slow_response.first->begin(), request_for_slow_response.first->end());
    messages.insert(messages.end(), fast_response.first->begin() + 1, fast_response.first->end() - 8);

    typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
    typedef DelimitedMessagesStreamParser<WrapperMessage>::PointerToConstValue PointerToConstValue;
    Parser parser;

    for (const char byte: messages)
    {
        parser.parse(string(1, byte));
    }

    auto parsedMessages = parser.getParsed();

    cout << "Parsed Messages: " << endl;
    for (const PointerToConstValue& value: parsedMessages)
    {
        cout << "message: ";

        if (value->has_request_for_slow_response())
        {
            cout << value->request_for_slow_response().time_in_seconds_to_sleep() << endl;
        }
        else if (value->has_fast_response())
        {
            cout << value->fast_response().current_date_time() << endl;
        }
        else if (value->has_request_for_fast_response())
        {
            cout << "Empty (must be empty)" << endl;
        }
    }
    return 0;
}
