#include "DelimitedMessagesStreamParser.h"
#include "cmath"
#include <memory>
#define MIN_UINT32_MSG_SIZE 4
#define MAX_UINT32_MSG_SIZE 8
#define RFFR_MSG_SIZE 2
#define FR_MSG_SIZE 23

using namespace std;

template<typename MessageType>
list<typename DelimitedMessagesStreamParser<MessageType>::PointerToConstValue> DelimitedMessagesStreamParser<MessageType>:: parse(conststring& data){
    size_t bytesConsumed = INVALID_BYTES_CONSUMED;
    int bufferNotChangedTimes = 0;
    m_buffer.insert(m_buffer.end(), data.begin(), data.end());

    while(!m_buffer.empty()) {
        shared_ptr<MessageType> new_mes = parseDelimited<MessageType>(&m_buffer, m_buffer.size(), &bytesConsumed);
        if (bytesConsumed != INVALID_BYTES_CONSUMED) {
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + (int) bytesConsumed);
            if (new_mes != nullptr) {
                parsed.push_back(new_mes);
            }
            bytesConsumed = INVALID_BYTES_CONSUMED;
            bufferNotChangedTimes = 0;
        } else {
            bufferNotChangedTimes++;
            if(bufferNotChangedTimes > max({FR_MSG_SIZE, RFFR_MSG_SIZE, MAX_UINT32_MSG_SIZE})+1) {
                  break;
            }
        }
    }

    return parsed;
};

template<typename MessageType>
list<typename DelimitedMessagesStreamParser<MessageType>::PointerToConstValue> & DelimitedMessagesStreamParser<MessageType>:: getParsed(){
    return parsed;
}

template<typename MessageType>
void DelimitedMessagesStreamParser<MessageType>::clear() {
    m_buffer.clear();
    parsed.clear();
}


template<typename Message>
shared_ptr<Message> parseDelimited(const void* data, size_t size, size_t* bytesConsumed){
    using namespace google::protobuf::io;
    regex fr_regex(R"(([12][0-9]{3})(0[1-9]|1[012])(0[1-9]|[12]\d|3[01])T(((0[1-9])|[^0]\d))?[0-5]\d[0-5]\d.\d\d\d)");

    auto  buffer = reinterpret_cast<vector<google::protobuf::uint8> *>(const_cast<void *>(data));
    boost::iostreams::basic_array_source<google::protobuf::uint8> bs(&(buffer->at(0)), buffer->size());
    boost::iostreams::stream<boost::iostreams::basic_array_source<google::protobuf::uint8>> is(bs);
    auto zcis = make_shared<IstreamInputStream>(reinterpret_cast<istream *>(&is));
    auto codedInputStream = make_shared<CodedInputStream>(zcis.get());

    google::protobuf::uint32 msgSize = 0;
    google::protobuf::uint32 sizeReadSuccess = codedInputStream->ReadVarint32(&msgSize);


    if((msgSize < size) && sizeReadSuccess) {
        shared_ptr<Message> result = make_shared<Message>();
        bool skipSuccess = false;
        bool dataReadSuccess = false;
        bool successfullyParsed = false;
        if ((msgSize >= MIN_UINT32_MSG_SIZE) && (msgSize <= MAX_UINT32_MSG_SIZE)) {
            google::protobuf::uint32 time_in_seconds_to_sleep;
            skipSuccess = codedInputStream->Skip(sizeof(msgSize) - 1);
            dataReadSuccess = codedInputStream->ReadVarint32(&time_in_seconds_to_sleep);
            if(skipSuccess && dataReadSuccess) {
                if (!isCorrectRangeVarint32(msgSize, time_in_seconds_to_sleep)) {
                    if (bytesConsumed != nullptr)
                        *bytesConsumed = msgSize + 1;
                    return nullptr;
                }
            }
            auto *sr = new RequestForSlowResponse();
            sr->set_time_in_seconds_to_sleep(time_in_seconds_to_sleep);
            result->set_allocated_request_for_slow_response(sr);
            successfullyParsed = true;
        } else if (msgSize == RFFR_MSG_SIZE) {
            string buf;
            skipSuccess = codedInputStream->Skip(1);
            dataReadSuccess = codedInputStream->ReadString(&buf, 1);
            if(skipSuccess && dataReadSuccess){
                if((int)buf[0] != 0) {
                    if (bytesConsumed != nullptr) {
                        *bytesConsumed = 1;
                        return nullptr;
                    }
                } else{
                    auto *rffr = new RequestForFastResponse();
                    result->set_allocated_request_for_fast_response(rffr);
                    successfullyParsed = true;
                }
            }

        } else if (msgSize == FR_MSG_SIZE) {
            string buf;
            skipSuccess = codedInputStream->Skip(sizeof(msgSize));
            dataReadSuccess = codedInputStream->ReadString(&buf, (int)(msgSize - sizeof(msgSize)));
            if(skipSuccess && dataReadSuccess) {
                if (!regex_match(buf, fr_regex)) {
                    *bytesConsumed = msgSize + 1;
                    return nullptr;
                }
            }
            auto *fr = new FastResponse();
            fr->set_current_date_time(move(buf));
            result->set_allocated_fast_response(fr);
            successfullyParsed= true;
        }

        if (successfullyParsed && (bytesConsumed != nullptr)) {
            *bytesConsumed = msgSize + 1;
            return move(result);
        } else if(!successfullyParsed && (bytesConsumed != nullptr)){
            *bytesConsumed = 1;
            return nullptr;
        }

    } else{
        if((sizeReadSuccess != 0)
           && (msgSize < MIN_UINT32_MSG_SIZE || msgSize > MAX_UINT32_MSG_SIZE)
           && (msgSize != FR_MSG_SIZE)
           && (msgSize != RFFR_MSG_SIZE)
           && (bytesConsumed != nullptr)){
            *bytesConsumed = 1;
            return nullptr;
        }
    }

    return nullptr;
}

template <typename Message>
PointerToConstData serializeDelimited(const Message& msg)
{
    const size_t messageSize = PROTOBUF_MESSAGE_BYTE_SIZE(msg);
    const size_t headerSize = google::protobuf::io::CodedOutputStream::VarintSize32(messageSize);
    const PointerToData& result = make_shared<Data>(headerSize + messageSize);
    google::protobuf::uint8* buffer = reinterpret_cast<google::protobuf::uint8*>(&*result->begin());

    google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(messageSize, buffer);
    msg.SerializeWithCachedSizesToArray(buffer + headerSize);

    return result;
}



bool isCorrectRangeVarint32(google::protobuf::uint32 msgSize, google::protobuf::uint32  result){
    switch (msgSize) {
        case 4:
            if(result >= (2, 7))
                return false;
            break;
        case 5:
            if(result < (2, 7) ||
            result >= (2, 14))
                return false;
            break;
        case 6:
            if(result < (2, 14) ||
               result >= (2, 21))
                return false;
            break;
        case 7:
            if(result < pow(2, 21) ||
               result >= pow(2, 28))
            return false;
            break;
        case 8:
            if(result < (2, 28))
            return false;
            break;
        default:
            break;
    }

    return true;
}

template< typename Buffer>
void printSerializedData(const void * data){
    auto  buffer = reinterpret_cast<Buffer *>(const_cast<void *>(data));
    cout << "[";
    for(int i = 0; i < buffer->size(); i++){
        cout << buffer->at(i);
    } cout << "]\n\n\n" << endl;
}

template PointerToConstData serializeDelimited(const WrapperMessage & msg);
template class DelimitedMessagesStreamParser<WrapperMessage>;
template shared_ptr<WrapperMessage> parseDelimited(const void* data, size_t size, size_t* bytesConsumed);
template void printSerializedData<vector<google::protobuf::uint8>>(const void * data);
