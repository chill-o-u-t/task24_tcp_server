TIMEOUT = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

TIMEOUT_ERROR_TO_INT = 'Timeout is empty or wrong, please use only {1 : 10} digits'
TIMEOUT_SET_DEFAULT = 'Timeout is wrong or empty, set default as 1'
TIME_SET_BY_USER = 'Timeout set at {time_out} seconds.'

HOST_EMPTY = 'Host is empty.'
WRONG_HOST = 'Introduced host is wrong: {host}'
PORT_EMPTY = 'Port is empty.'
WRONG_PORT_LEN = 'Len of port ({len}) more then max port len'
WRONG_PORT = 'Can`t convert string port to int'

DELAY_ERROR = 'Can`t set delay more 1000 or less 10'
DELAY_IS_OK = 'Delay set at {delay} sec.'

SEND_SLOW_MESSAGE = 'Slow request message is sending now'
ERROR_DATA_MESSAGE = 'Data sending failed: {error}'
SEND_FAST_MESSAGE = 'Fast request message is sending now'

CONNECTION_IS_OK = 'Successful connection: {host}:{port}'
CONNECTION_FAILED = 'Connection failed: {error}'

RECEIVED_DATA_ERROR = 'Can`t decode message with protobuf: {error}'
SUCCESSFULLY_RECEIVED = 'Successful data received'
