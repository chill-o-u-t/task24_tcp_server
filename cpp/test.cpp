Ivan Morozov, [27.03.2023 18:47]
#include <gtest/gtest.h>
#include "test_api.h"



TEST(testParseDelimited, test_1) { // incorrect buffer parsing
    std::vector<char> buffer;

    std::pair<PointerToConstData, WrapperMessage> fr = TestApi::getFastResponse(false);
    std::pair<PointerToConstData, WrapperMessage> rffr = TestApi::getRequestForFastResponse();
    std::pair<PointerToConstData, WrapperMessage> rfsr = TestApi::getRequestForSlowResponse(512);
    std::pair<PointerToConstData, WrapperMessage> frWrongFormat = TestApi::getFastResponse(true);

    std::string r = "PTembLOJVwVR809aP7qpKnXtNEFVqiykxXOly6itwhOaJjB1cidNdcJFIGWkcyZGOt1OyQ0pNVK4Z9jZXG6UDcpPTCvyODfNQrA3qmZvWdFpPpr31FT5KaUxCtSgtvFc";
    buffer.insert(buffer.end(), r.begin(), r.begin() + 10);
    buffer.insert(buffer.end(), fr.first->begin(), fr.first->end());
    buffer.insert(buffer.end(), r.begin() + 10, r.begin() + 25);
    size_t bytesConsumed = INVALID_BYTES_CONSUMED;
    auto result = parseDelimited<WrapperMessage>(&buffer, buffer.size(), &bytesConsumed);
    ASSERT_EQ(result, nullptr);

    clear( &buffer, &bytesConsumed);
    buffer.insert(buffer.end(), fr.first->begin()+1, fr.first->end());
    result = parseDelimited<WrapperMessage>(&buffer, buffer.size(), &bytesConsumed);
    ASSERT_EQ(result, nullptr);

    clear(&buffer, &bytesConsumed);
    buffer.insert(buffer.end(), rffr.first->begin()+1, rffr.first->end()-1);
    result = parseDelimited<WrapperMessage>(&buffer, buffer.size(), &bytesConsumed);
    ASSERT_EQ(result, nullptr);

    clear(&buffer, &bytesConsumed);
    buffer.insert(buffer.end(), rfsr.first->begin(), rfsr.first->begin()+3);
    result = parseDelimited<WrapperMessage>(&buffer, buffer.size(), &bytesConsumed);
    ASSERT_EQ(result, nullptr);

    clear(&buffer, &bytesConsumed);
    buffer.insert(buffer.end(), frWrongFormat.first->begin(), frWrongFormat.first->end());
    result = parseDelimited<WrapperMessage>(&buffer, buffer.size(), &bytesConsumed);
    ASSERT_EQ(result, nullptr);
}

TEST(testParseDelimited, test_2){ // correct messages parsing
    std::vector<char> buffer;

    std::pair<PointerToConstData, WrapperMessage> fr = TestApi::getFastResponse(false);
    std::pair<PointerToConstData, WrapperMessage> rffr = TestApi::getRequestForFastResponse();
    std::pair<PointerToConstData, WrapperMessage> rfsr = TestApi::getRequestForSlowResponse(512);
    size_t bytesConsumed = INVALID_BYTES_CONSUMED;

    buffer.insert(buffer.end(), fr.first->begin(), fr.first->end());
    auto result = parseDelimited<WrapperMessage>(&buffer, buffer.size(), &bytesConsumed);
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result->has_fast_response(), true);
    ASSERT_STREQ(result->fast_response().current_date_time().c_str(), fr.second.fast_response().current_date_time().c_str());

    clear(&buffer, &bytesConsumed);
    buffer.insert(buffer.end(), rffr.first->begin(), rffr.first->end());
    result = parseDelimited<WrapperMessage>(&buffer, buffer.size(), &bytesConsumed);
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result->has_request_for_fast_response(), true);
    ASSERT_EQ(result->request_for_fast_response().GetTypeName(), rffr.second.request_for_fast_response().GetTypeName());

    clear(&buffer, &bytesConsumed);
    buffer.insert(buffer.end(), rfsr.first->begin(), rfsr.first->end());
    result = parseDelimited<WrapperMessage>(&buffer, buffer.size(), &bytesConsumed);
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result->has_request_for_slow_response(), true);
    ASSERT_EQ(result->request_for_slow_response().time_in_seconds_to_sleep(), rfsr.second.request_for_slow_response().time_in_seconds_to_sleep());

}


TEST(testDelimitedMessagesStreamParser, test_1){
    std::vector<char> messages;
    auto fr = TestApi::getFastResponse(false);
    auto  rffr = TestApi::getRequestForFastResponse();
    auto  rfsr = TestApi::getRequestForSlowResponse(512);
    bool lastTime = false;
    messages.insert(messages.end(), (fr.first)->begin()+1, (fr.first)->begin()+3); // rubbish
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()); // 1
    messages.insert(messssages.end(), fr.first->begin(), fr.first->end()); // 2
    messages.insert(messages.end(), fr.first->begin() + 5, fr.first->begin() + 10); // rubbish
    messages.insert(messages.end(), rfsr.first->begin(), rfsr.first->end()); // 3
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()); // 4
    messages.insert(messages.end(), rfsr.first->begin(), rfsr.first->begin() + 4); // rubbish

    int expected_messages_count = 4;

    Parser parser;

    for(const char byte : messages) {
        parser.parse(std::string(1, byte));
    }

    auto parsedMessages = parser.getParsed();

    AGAIN:
    ASSERT_EQ(parsedMessages.size(), expected_messages_count);
    // here we test only three types of messages cause we identify message type by message size
    // so SlowResponse and RequestForSlowResponse have the same size and this parser-102 is for server.
    // Server cannot get SlowResponse, only request for slow response.
    auto current = parsedMessages.begin();
    ASSERT_EQ(current->get()->has_request_for_fast_response(), true);
    ASSERT_EQ(current->get()->request_for_fast_response().GetTypeName(),
              rffr.second.request_for_fast_response().GetTypeName());

    current++;
    ASSERT_EQ(current->get()->has_fast_response(), true);
    ASSERT_STREQ(current->get()->fast_response().current_date_time().c_str(),
              fr.second.fast_response().current_date_time().c_str());

    current++;
    ASSERT_EQ(current->get()->has_request_for_slow_response(), true);
    ASSERT_EQ(current->get()->request_for_slow_response().time_in_seconds_to_sleep(),
              rfsr.second.request_for_slow_response().time_in_seconds_to_sleep());

    current++;
    ASSERT_EQ(current->get()->has_request_for_fast_response(), true);
    ASSERT_EQ(current->get()->request_for_fast_response().GetTypeName(),
              rffr.second.request_for_fast_response().GetTypeName());
    if(lastTime)
        goto EXIT;

    parser.clear();
    parser.parse(std::string(messages.begin(), messages.end()));
    parsedMessages = parser.getParsed();
    lastTime = true;
    goto AGAIN;

    EXIT:{};
}

TEST(testDelimitedMessagesStreamParser, test_2){
    std::vector<char> messages;

    std::pair<PointerToConstData, WrapperMessage> fr = TestApi::getFastResponse(false);
    std::pair<PointerToConstData, WrapperMessage> rffr = TestApi::getRequestForFastResponse();
    std::pair<PointerToConstData, WrapperMessage> rfsr = TestApi::getRequestForSlowResponse(512);
    bool lastTime = false;


    messages.insert(messages.end(), (fr.first)->begin()+1, (fr.first)->begin()+3); // rubbish
    messages.insert(messages.end(), fr.first->begin(), fr.first->end()); // 1 fr
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()); // 2 rffr
    messages.insert(messages.end(), fr.first->begin() + 5, fr.first->begin() + 10); // rubbish
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()); // 3 rffr
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()-1); // rubbish
    messages.insert(messages.end(), rfsr.first->begin(), rfsr.first->end()); // 4 rfsr
    messages.insert(messages.end(), rfsr.first->begin() + 2, rfsr.first->end() + 5); // rubbish
    messages.insert(messages.end(), rfsr.first->begin()+1, rfsr.first->end()); // rubbish
    messages.insert(messages.end(), rfsr.first->begin(), rfsr.first->end() + 3); // rubbish
    messages.insert(messages.end(), fr.first->begin(), fr.first->end()); // 5 fr
    messages.insert(messages.end(), rffr.first->begin()+1, rffr.first->begin()+3); // rubbish

    Parser parser;
    for(const char byte : messages) {
        parser.parse(std::string(1, byte));
    }

    auto parsedMessages = parser.getParsed();

    AGAIN:
    int expected_messages_count = 5;
    ASSERT_EQ(parsedMessages.size(), expected_messages_count);

    auto current = parsedMessages.begin();
    ASSERT_EQ(current->get()->has_fast_response(), true);
    ASSERT_STREQ(current->get()->fast_response().current_date_time().c_str(),
              fr.second.fast_response().current_date_time().c_str());

    current++;
    ASSERT_EQ(current->get()->has_request_for_fast_response(), true);
    ASSERT_EQ(current->get()->request_for_fast_response().GetTypeName(),
              rffr.second.request_for_fast_response().GetTypeName());

    current++;
    ASSERT_EQ(current->get()->has_request_for_fast_response(), true);
    ASSERT_EQ(current->get()->request_for_fast_response().GetTypeName(),
              rffr.second.request_for_fast_response().GetTypeName());

    current++;
    ASSERT_EQ(current->get()->has_request_for_slow_response(), true);
    ASSERT_EQ(current->get()->request_for_slow_response().time_in_seconds_to_sleep(),
              rfsr.second.request_for_slow_response().time_in_seconds_to_sleep());

    current++;
    ASSERT_EQ(current->get()->has_fast_response(), true);
    ASSERT_STREQ(current->get()->fast_response().current_date_time().c_str(),
              fr.second.fast_response().current_date_time().c_str());

    if(lastTime)
        goto EXIT;

    parser.clear();
    parser.parse(std::string(messages.begin(), messages.end()));
    parsedMessages = parser.getParsed();
    lastTime = true;
    goto AGAIN;

    EXIT:{};

}

TEST(testDelimitedMessagesStreamParser, test_3){
    std::vector<char> messages;
    bool lastTime = false;

    std::pair<PointerToConstData, WrapperMessage> fr = TestApi::getFastResponse(false);
    std::pair<PointerToConstData, WrapperMessage> rffr = TestApi::getRequestForFastResponse();
    std::pair<PointerToConstData, WrapperMessage> rfsr = TestApi::getRequestForSlowResponse(512);
    std::pair<PointerToConstData, WrapperMessage> frWrongFormat = TestApi::getFastResponse(true);

    std::string r = "PTembLOJVwVR809aP7qpKnXtNEFVqiykxXOly6itwhOaJjB1cidNdcJFIGWkcyZGOt1OyQ0pNVK4Z9jZXG6UDcpPTCvyODfNQrA3qmZvWdFpPpr31FT5KaUxCtSgtvFc";
    messages.insert(messages.end(), r.begin(), r.end()-80);
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()); // 1 rffr
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()-2);
    messages.insert(messages.end(), r.begin()+5, r.end()-10);
    messages.insert(messages.end(), fr.first->begin(), fr.first->end()-1);
    messages.insert(messages.end(), r.begin()+5, r.end()-10);
    messages.insert(messages.end(), fr.first->begin(), fr.first->end()); // 2 fr
    messages.insert(messages.end(), fr.first->begin()+1, fr.first->end()-8);
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()-2);
    messages.insert(messages.end(), r.begin()+50, r.end()-20);
    messages.insert(messages.end(), frWrongFormat.first->begin(), frWrongFormat.first->end()); //  fr wrong
    messages.insert(messages.end(), fr.first->begin(), fr.first->end()); // 3 fr
    messages.insert(messages.end(), r.begin()+50, r.end()-20);
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()-2);
    messages.insert(messages.end(), r.begin()+70, r.end()-20);
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()-1);
    messages.insert(messages.end(), r.begin()+70, r.end()-20);
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()); // 4 rffr
    messages.insert(messages.end(), rffr.first->begin(), rffr.first->end()-1);
    messages.insert(messages.end(), r.begin()+70, r.end()-20);

    Parser parser;
    for(const char byte : messages) {
        parser.parse(std::string(1, byte));
    }

    AGAIN:
    auto parsedMessages = parser.getParsed();
    int expected_messages_count = 4;
    ASSERT_EQ(parsedMessages.size(), expected_messages_count);

    auto current = parsedMessages.begin();
    ASSERT_EQ(current->get()->has_request_for_fast_response(), true);
    ASSERT_EQ(current->get()->request_for_fast_response().GetTypeName(),
              rffr.second.request_for_fast_response().GetTypeName());

    current++;
    ASSERT_EQ(current->get()->has_fast_response(), true);
    ASSERT_STREQ(current->get()->fast_response().current_date_time().c_str(),
              fr.second.fast_response().current_date_time().c_str());

    current++;
    ASSERT_EQ(current->get()->has_fast_response(), true);
    ASSERT_STREQ(current->get()->fast_response().current_date_time().c_str(),
              fr.second.fast_response().current_date_time().c_str());

    current++;
    ASSERT_EQ(current->get()->has_request_for_fast_response(), true);
    ASSERT_EQ(current->get()->request_for_fast_response().GetTypeName(),
              rffr.second.request_for_fast_response().GetTypeName());

    if(lastTime)
        goto EXIT;

    parser.clear();
    parser.parse(std::string(messages.begin(), messages.end()));
    parsedMessages = parser.getParsed();
    lastTime = true;
    goto AGAIN;

    EXIT:{};
}


TEST(testRange, test_1){
    ASSERT_EQ(isCorrectRangeVarint32(4, 128), false);
    ASSERT_EQ(isCorrectRangeVarint32(4, 127), true);

    ASSERT_EQ(isCorrectRangeVarint32(5, 8000), true);
    ASSERT_EQ(isCorrectRangeVarint32(5, 17000), false);

    ASSERT_EQ(isCorrectRangeVarint32( 6, std::pow(2, 20)), true);
    ASSERT_EQ(isCorrectRangeVarint32(6, std::pow(2, 21)), false);
//
    ASSERT_EQ(isCorrectRangeVarint32(7 , std::pow(2, 22)), true);
    ASSERT_EQ(isCorrectRangeVarint32(7, std::pow(2, 28)), false);
//
    ASSERT_EQ(isCorrectRangeVarint32(8 , std::pow(2 , 28)), true);
    ASSERT_EQ(isCorrectRangeVarint32(8 , std::pow(2 , 28)-1), false);
}