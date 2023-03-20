import logging

LOG_FORMAT = (
    "[%(asctime)s]:[%(filename)s:%(lineno)d] "
    "- %(levelname)s - %(name)s - %(message)s"
)


class CustomLogFormatter(logging.Formatter):
    magenta = "\x1b[95m"
    cyan = "\x1b[96m"
    yellow = "\x1b[93m"
    red = "\x1b[31m"
    bold_red = "\x1b[31;1m"
    reset = "\x1b[0m"

    FORMATS = {
        logging.DEBUG: magenta + LOG_FORMAT + reset,
        logging.INFO: cyan + LOG_FORMAT + reset,
        logging.WARNING: yellow + LOG_FORMAT + reset,
        logging.ERROR: red + LOG_FORMAT + reset,
        logging.CRITICAL: bold_red + LOG_FORMAT + reset
    }

    def format(self, message):
        log_fmt = self.FORMATS.get(message.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(message)
