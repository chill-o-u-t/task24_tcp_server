from google.protobuf.internal.encoder import _VarintBytes
from google.protobuf.internal.decoder import _DecodeVarint32

from python.user.tcp_connection_pb2 import (
    WrapperMessage,
    SlowResponse,
    RequestForSlowResponse,
    RequestForFastResponse
)


@classmethod
class Parser:
    _buffer: bytes = b''
    start: int = 0
    message: WrapperMessage = WrapperMessage()
    reader: object = ''  # some function to receive data
    writer: object = ''  # some function to send data

    def server_parser(self):
        data = self.reader  # get data from the stream
        self._buffer += data

        message_size, position = _DecodeVarint32(self._buffer, self.start)
        current_message = self._buffer[position:(message_size + position)]
        self.message.ParseFromString(current_message)
        self._buffer = self._buffer[(message_size + position):]

        if self.message.HasField('request_for_fast_response'):
            self.message.fast_response.current_date_time = 'time now'
            self.writer.write(_VarintBytes(self.message.ByteSize()))  # write size of message
            self.writer.write(self.message.SerializeToString())  # send message
        if self.message.HasField('request_for_slow_response'):
            users_count = 1  # get count of connections
            instance = SlowResponse()
            instance.connected_client_count = users_count
            msg = WrapperMessage()
            msg.slow_response.CopyFrom(instance)
            # some function to sleep there
            self.writer.write(_VarintBytes(self.message.ByteSize()))  # write size of message
            self.writer.write(msg.SerializeToString())  # send message

    def ui_parser(self):
        data = self.reader  # get data from the stream
        self._buffer += data

        message_size, position = _DecodeVarint32(self._buffer, self.start)
        current_message = self._buffer[position:(message_size + position)]
        self.message.ParseFromString(current_message)
        self._buffer = self._buffer[(message_size + position):]
        if self.message.HasField('fast_response'):
            ...
            # funtion to set current_message in UI
        if self.message.HasField('slow_response'):
            ...
            # funtion to set current_message in UI

    def ui_create_message(self):
        # slow reponse
        instance = RequestForSlowResponse()
        instance.time_in_seconds_to_sleep = self.check_delay()  # function to get delay from UI
        self.message.request_for_slow_response.CopyFrom(instance)
        self.writeer.write(self.message.SerializeToString())  # some function to send message from ui

        # fast response
        instance = RequestForFastResponse()
        self.message.request_for_fast_response.CopyFrom(instance)
        self.writer.write(_VarintBytes(self.message.ByteSize()))  # some function to encode message size to varint32
        self.writer.write(self.message.SerializeToString())  # some function to send message from ui
