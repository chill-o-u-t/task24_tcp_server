import unittest
import socket

import tcp_connection_pb2


class TestClient(unittest.TestCase):
    def setUp(self) -> None:
        self.HOST = 'localhost'
        self.port = 9999
        self.message = tcp_connection_pb2.WrapperMessage
        self.answer = tcp_connection_pb2.WrapperMessage
        self.fake_server = socket.socket()
        self.fake_server.connect((self.HOST, self.port))

    def tearDown(self) -> None:
        self.fake_server.close()

    def test_request_for_fast_response(self):
        instance = tcp_connection_pb2.FastResponse
        self.message.fast_response.CopyFrom(instance)
        self.fake_server.send(self.message.SerializeToString())
        data = self.fake_server.recv(1024)
        self.answer.ParseFromString(data)
        self.assertTrue(self.answer.HasField('request_for_fast_response'))

    def test_request_for_fast_response(self):
        instance = tcp_connection_pb2.FastResponse
        instance.connected_client_count = 1
        self.message.slow_response.CopyFrom(instance)
        self.fake_server.send(self.message.SerializeToString())
        data = self.fake_server.recv(1024)
        self.answer.ParseFromString(data)
        self.assertTrue(self.answer.HasField('request_for_slow_response'))


if __name__ == '__main__':
    unittest.main()
