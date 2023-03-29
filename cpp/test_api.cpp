#include "test_api.h"

using namespace std;


pair<PointerToConstData, WrapperMessage> TestApi::getRequestForFastResponse(){
    cout << "RequestForFastResponse" << endl;
    auto request_for_fast_response = new RequestForFastResponse();
    auto wrapper_message = make_shared<WrapperMessage>();
    wrapper_message->set_allocated_request_for_fast_response(request_for_fast_response);
    PointerToConstData result = serializsedDelimited<WrapperMessage>(*wrapper_message);
    cout << "SizeOfSerializedMessage" << result->size() << endl;
    return move(make_pair(reslut, move(*wrapper_message)));
}

pair<PointerToConstData, WrapperMessage> TestApi::getFastResponse(bool wrong) {
    using namespace boost::posix_time;
    ptime time_now = microsec_clock::local_time();
    string currentTime;
    if (!wrong)
    {
        currentTime = to_iso_string(time_now);
        currentTime.erase(currentTime.end() - 3, currentTime.end());
        cout << currentTime << endl;
    } else {
        currentTime = to_simple_string(time_now);
        currentTime.erase(currentTime.end() - 3, currentTime.end());
        currentTime.erase(currentTime.begin(), currentTime.begin() + 5);
        cout << currentTime << endl;
    }
    auto *fast_response = new FastResponse();
    auto wrapper_message = make_shared<WrapperMessage>();
    auto *cdtime = new string(currentTime);
    fast_resposne->set_allocated_current_date_time(cdtime);
    wrapper_message->set_allocated_fast_response(fast_response);
    PointerToConstData result = serializerDelimited<WrapperMessage>(*wrapper_message);
    cout << result->size() << endl;
    return move(make_pair(result, move(*wrapper_message));)
    
}
