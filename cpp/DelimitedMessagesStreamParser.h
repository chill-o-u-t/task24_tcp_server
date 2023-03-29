//#ifndef TASK24_TCP_SERVER_DELIMITEDMESSAGESSTREAMPARSER_H
//#define TASK24_TCP_SERVER_DELIMITEDMESSAGESSTREAMPARSER_H
#include "message.pb.h"
#include <vector>
#include <list>
#include <boost/make_shared.hpp>
#include <istream>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <regex>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <limits.h>
#define MIN_UINT32_MSG_SIZE 4
#define MAX_UINT32_MSG_SIZE 8
#define RFFR_MSG_SIZE 2
#define FR_MSG_SIZE 23


typedef std::vector<char> Data;
typedef boost::shared_ptr<const Data> PointerToConstData;
typedef boost::shared_ptr<Data> PointerToData;
typedef TestTask::Messages::WrapperMessage WrapperMessage;
typedef TestTask::Messages::FastResponse FastResponse;
typedef TestTask::Messages::SlowResponse SlowResponse;
typedef TestTask::Messages::RequestForFastResponse RequestForFastResponse;
typedef TestTask::Messages::RequestForSlowResponse RequestForSlowResponse;


template <typename Message>
PointerToConstData SerializeDelimited(const Message msg);

template <typename Message>
boost::shared_ptr<Message> ParserDelimited(const void* data, size_t size, size_t* bytesConsumed = nullptr);

template <typename MessageType>
class DelimitedMessagesStreamParser {
    public:
        typedef boost::shared_ptr<const MessageType> PointerToConstValue;
        std::list<PointerToConstValue> parse(const std::string data);
        std::list<PointerToConstValue> getParsed();
    
    private:
    std::vector<char> buffer;
    std::list<PointerToConstValue> parsed;
};

template <typename Buffer>
void PrintSerializedData(const void* data);
bool isCorrectRangeVarint32(google::protobuf::uint msgSize, google::protobuf::uint32 result);
