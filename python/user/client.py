import logging
import re

from google.protobuf.internal.decoder import _DecodeVarint32
from google.protobuf.internal.encoder import _VarintBytes

import tcp_connection_pb2

from user.constants import *
from logger_config import CustomLogFormatter


macbook = True


if macbook:
    from PyQt6 import QtCore, QtWidgets
    from PyQt6.QtGui import QTextCursor
    from PyQt6.QtNetwork import QTcpSocket
    from PyQt6.QtWidgets import QDialog
else:
    from PyQt5 import QtCore, QtWidgets
    from PyQt5.QtGui import QTextCursor
    from PyQt5.QtNetwork import QTcpSocket
    from PyQt5.QtWidgets import QDialog


DEBUG = True


class Client(QDialog):
    def __init__(self) -> None:
        super().__init__()
        self._buffer = b''
        self.start = 0
        self.message = tcp_connection_pb2.WrapperMessage()
        self.time_out = 1000
        self.tcp_socket = QTcpSocket(self)
        self.tcp_socket.readyRead.connect(self.deal_communication)

    @staticmethod
    def is_empty(data) -> bool:
        """
        Проверка объекта на наличие данных,
        если объект пуст или равен 0 возвращает False.
        :param data:
        :return:
        """
        return data == 0 or data == ''

    @staticmethod
    def check_ip(ip) -> bool:
        """
        Проверка введенного ip адреса на соответствие формату.
        :param ip:
        :return:
        """
        if ip == 'localhost':
            return True
        if re.match(
            '\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}', ip
        ) is None:
            return False
        for block in map(int, ip.split('.')):
            if block > 255:
                return False
        return True


class UiMainWindow(Client):
    def setup_ui(self, main_window):
        """
        Установка элементов интерфейса, расположение в окне
        :param main_window:
        :return:
        """
        main_window.setObjectName("MainWindow")
        main_window.resize(800, 600)
        self.central_widget = QtWidgets.QWidget(main_window)
        self.central_widget.setObjectName("central-widget")

        # text_edits
        self.text_edit_host = QtWidgets.QTextEdit(self.central_widget)
        self.text_edit_host.setGeometry(QtCore.QRect(210, 50, 104, 40))
        self.text_edit_host.setObjectName("textEdit")
        self.text_edit_port = QtWidgets.QTextEdit(self.central_widget)
        self.text_edit_port.setGeometry(QtCore.QRect(370, 50, 104, 40))
        self.text_edit_port.setObjectName("textEdit_2")
        self.text_edit_timeout = QtWidgets.QTextEdit(self.central_widget)
        self.text_edit_timeout.setGeometry(QtCore.QRect(40, 130, 104, 40))
        self.text_edit_timeout.setObjectName("textEdit_3")
        self.text_edit_delay = QtWidgets.QTextEdit(self.central_widget)
        self.text_edit_delay.setGeometry(QtCore.QRect(210, 290, 104, 40))
        self.text_edit_delay.setObjectName("textEdit_4")

        # labels
        self.label_ip = QtWidgets.QLabel(self.central_widget)
        self.label_ip.setGeometry(QtCore.QRect(210, 30, 100, 13))
        self.label_ip.setObjectName("label")
        self.label_port = QtWidgets.QLabel(self.central_widget)
        self.label_port.setGeometry(QtCore.QRect(370, 30, 100, 13))
        self.label_port.setObjectName("label_2")
        self.label_connected_status = QtWidgets.QLabel(self.central_widget)
        self.label_connected_status.setGeometry(
            QtCore.QRect(520, 50, 170, 40)
        )
        self.label_connected_status.setObjectName("label_3")
        self.label_timeout = QtWidgets.QLabel(self.central_widget)
        self.label_timeout.setGeometry(QtCore.QRect(40, 110, 100, 13))
        self.label_timeout.setObjectName("label_4")
        self.label_default_timeout = QtWidgets.QLabel(self.central_widget)
        self.label_default_timeout.setGeometry(QtCore.QRect(40, 170, 100, 13))
        self.label_default_timeout.setObjectName("label_5")
        self.label_delay = QtWidgets.QLabel(self.central_widget)
        self.label_delay.setGeometry(QtCore.QRect(210, 270, 100, 13))
        self.label_delay.setObjectName("label_6")

        # outputs
        self.label_output_slow = QtWidgets.QLabel(self.central_widget)
        self.label_output_slow.setGeometry(QtCore.QRect(380, 290, 310, 40))
        self.label_output_slow.setObjectName("label_7")
        self.label_output_fast = QtWidgets.QLabel(self.central_widget)
        self.label_output_fast.setGeometry(QtCore.QRect(380, 210, 310, 40))
        self.label_output_fast.setObjectName("label_8")
        self.label_output_fast_msg = QtWidgets.QLabel(self.central_widget)
        self.label_output_fast_msg.setGeometry(QtCore.QRect(470, 210, 310, 40))
        self.label_output_fast_msg.setObjectName("label_9")
        self.label_output_slow_msg = QtWidgets.QLabel(self.central_widget)
        self.label_output_slow_msg.setGeometry(QtCore.QRect(470, 290, 310, 40))
        self.label_output_slow_msg.setObjectName("label_10")

        # status
        self.label_status = QtWidgets.QLabel(self.central_widget)
        self.label_status.setGeometry(QtCore.QRect(210, 100, 100, 13))
        self.label_status.setObjectName("status")

        # buttons
        self.push_button_fast = QtWidgets.QPushButton(self.central_widget)
        self.push_button_fast.setGeometry(QtCore.QRect(40, 210, 111, 41))
        self.push_button_fast.setObjectName("pushButton_2")
        self.push_button_slow = QtWidgets.QPushButton(self.central_widget)
        self.push_button_slow.setGeometry(QtCore.QRect(40, 290, 111, 41))
        self.push_button_slow.setObjectName("pushButton_3")

        # logger
        self.logger_console = QtWidgets.QTextEdit(self.central_widget)
        self.logger_console.setGeometry(QtCore.QRect(40, 361, 641, 211))
        self.logger_console.setObjectName("textBrowser")
        self.logger_console.setStyleSheet(
            """
            QTextEdit {
                background-color: #000;
                color: #00ff00
            }"""
        )
        sys.stdout.write = self.request_std(sys.stdout.write)
        sys.stderr.write = self.request_std(sys.stderr.write)

        # other
        self.checkbox = QtWidgets.QCheckBox(main_window)
        self.checkbox.setGeometry(QtCore.QRect(500, 50, 1111, 41))
        self.checkbox.setObjectName('connection')
        main_window.setCentralWidget(self.central_widget)
        self.menubar = QtWidgets.QMenuBar(main_window)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 21))
        self.menubar.setObjectName("menubar")
        main_window.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(main_window)
        self.statusbar.setObjectName("statusbar")
        main_window.setStatusBar(self.statusbar)
        self.retranslate_ui(main_window)
        QtCore.QMetaObject.connectSlotsByName(main_window)
        logging.info('Interface created')

    def retranslate_ui(self, main_window):
        """
        Установка графическим элементам параметров (текст, функции для кнопок)
        :param main_window:
        :return:
        """
        _translate = QtCore.QCoreApplication.translate
        main_window.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.label_ip.setText(_translate("MainWindow", "IP address"))
        self.label_port.setText(_translate("MainWindow", "Port"))
        self.label_connected_status.setText(
            _translate("MainWindow", "Successfully connection or error")
        )
        self.push_button_fast.setText(_translate("MainWindow", "Fast Request"))
        self.push_button_slow.setText(_translate("MainWindow", "Slow Request"))
        self.label_timeout.setText(_translate("MainWindow", "TimeOut"))
        self.label_default_timeout.setText(
            _translate("MainWindow", "Default TimeOut: 1")
        )
        self.label_delay.setText(_translate("MainWindow", "Delay"))
        self.label_output_fast.setText(
            _translate("MainWindow", "output here")
        )
        self.label_output_slow.setText(
            _translate("MainWindow", "output here")
        )
        self.push_button_slow.clicked.connect(self.slow_request)
        self.push_button_fast.clicked.connect(self.fast_request)
        self.checkbox.clicked.connect(self.check_data_host_and_port)

    def check_timeout(self) -> None:
        """
        Проверяет timeout на соответствие условию: от 1 до 10 сек.
        Проверка, что timeout, установленный пользователем - число.
        Если таймаут не задан, то устанавливается стандартный 1 сек.
        :return:
        """
        timeout = self.text_edit_timeout.toPlainText()
        try:
            timeout_digit = int(timeout)
        except ValueError:
            logging.info(TIMEOUT_ERROR_TO_INT)
            return
        if timeout_digit in TIMEOUT:
            self.time_out = int(self.text_edit_timeout.toPlainText())
            logging.info(TIME_SET_BY_USER.format(timeout=self.time_out))
            return
        logging.info(TIMEOUT_SET_DEFAULT)
        return

    def check_data_host_and_port(self) -> None:
        """
        Проверяет корректность введеных Ip и Port.
        host: является вида ???.???.???.??? или localhost.
        Числа в ip не превышают 255.
        Значение поля host не является пустым или строкой (кроме localhost).
        port: является числом в диапозоне от 1 до 9999
        :return:
            None, если есть ошибка
            make_request(host, port) если все данные прошли валидацию.
        """
        if DEBUG:
            self.make_request('localhost', 9999)
            self.label_connected_status.setText('Connected')
            return
        host = self.text_edit_host.toPlainText()
        text_port = self.text_edit_port.toPlainText()
        if self.is_empty(host):
            self.checkbox.setChecked(False)
            self.label_connected_status.setText('Host is none')
            logging.error(HOST_EMPTY)
            return
        if self.is_empty(text_port):
            self.checkbox.setChecked(False)
            self.label_connected_status.setText('Port is None')
            logging.error(PORT_EMPTY)
            return
        if not self.check_ip(host):
            self.checkbox.setChecked(False)
            self.label_connected_status.setText('Invalid IP')
            logging.error(WRONG_HOST.format(host=host))
            return
        if len(text_port) > 4:
            self.checkbox.setChecked(False)
            self.label_connected_status.setText('Invalid Port')
            logging.error(
                WRONG_PORT_LEN.format(len=len(text_port))
            )
            return
        try:
            port = int(text_port)
        except ValueError:
            self.label_connected_status.setText('Port error')
            logging.error(WRONG_PORT)
            return
        self.label_connected_status.setText('Connected')
        self.make_request(host, port)

    def check_delay(self) -> int:
        """
        Проверяется, что установленный delay в диапозоне от 10 до 1000.
        :return:
            int, в диапозоне от 1 до 100 (сек)
        """
        delay_text = self.text_edit_delay.toPlainText()
        if self.is_empty(delay_text):
            delay = 10
        else:
            delay = int(delay_text)
        if delay < 10 or delay > 1000:
            logging.error(DELAY_ERROR)
            return 1
        logging.info(DELAY_IS_OK.format(delay=(delay // 10)))
        return delay // 10

    def slow_request(self) -> None:
        if not self.checkbox.isChecked():
            return
        instance = tcp_connection_pb2.RequestForSlowResponse()
        try:
            instance.time_in_seconds_to_sleep = self.check_delay()
            self.message.request_for_slow_response.CopyFrom(instance)
            self.tcp_socket.write(self.message.SerializeToString())
            self.message.Clear()
            logging.info(SEND_SLOW_MESSAGE)
        except Exception as error:
            logging.error(ERROR_DATA_MESSAGE.format(error=error))

    def fast_request(self) -> None:
        if not self.checkbox.isChecked():
            return
        instance = tcp_connection_pb2.RequestForFastResponse()
        try:
            self.message.request_for_fast_response.CopyFrom(instance)
            self.tcp_socket.write(_VarintBytes(self.message.ByteSize()))
            self.tcp_socket.write(self.message.SerializeToString())
            logging.info(SEND_FAST_MESSAGE)
            self.message.Clear()
        except Exception as error:
            logging.error(ERROR_DATA_MESSAGE.format(error=error))

    def make_request(self, host, port) -> None:
        self.check_timeout()
        try:
            self.tcp_socket.connectToHost(host, port)
            logging.info(
                CONNECTION_IS_OK.format(host=host, port=port)
            )
            self.label_status.setText(f'{host}:{port}')
        except Exception as error:
            logging.error(CONNECTION_FAILED)

    def deal_communication(self) -> None:
        self._buffer = bytes(self.tcp_socket.readAll())
        message_size, pos = _DecodeVarint32(self._buffer, self.start)
        current_message = self._buffer[pos:(message_size + pos)]
        self.message.ParseFromString(current_message)
        self._buffer = self._buffer[pos + message_size:]
        try:
            if self.message.HasField('fast_response'):
                self.label_output_fast_msg.setText(
                    self.message.fast_response.current_date_time
                )
                logging.info(SUCCESSFULLY_RECEIVED)
            if self.message.HasField('slow_response'):
                self.label_output_slow_msg.setText(
                    f'{self.message.slow_response.connected_client_count}'
                )
                logging.info(SUCCESSFULLY_RECEIVED)
        except Exception as error:
            logging.error(RECEIVED_DATA_ERROR.format(error=error))
        finally:
            self.message.Clear()
            self._buffer = b''

    def request_std(self, func):
        """
        Вывод логов в консоль графического приложения.
        :param func:
        :return:
        """

        def inner(inputStr):
            cursor = QTextCursor(self.logger_console.document())
            cursor.setPosition(0)
            self.logger_console.setTextCursor(cursor)
            self.logger_console.insertPlainText(inputStr)
            return func(inputStr)

        return inner


if __name__ == "__main__":
    import sys

    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)
    stream_handler = logging.StreamHandler()
    stream_handler.setFormatter(CustomLogFormatter())
    stream_handler.setLevel(logging.DEBUG)
    logger.addHandler(stream_handler)
    logger = logging.getLogger("TCP_client")

    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = UiMainWindow()
    ui.setup_ui(MainWindow)
    MainWindow.show()
    logging.info('Started client')

    sys.exit(app.exec())
