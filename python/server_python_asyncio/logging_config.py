import logging
from logging.handlers import RotatingFileHandler
from pathlib import Path

BASE_DIR = Path(__file__).parent
LOG_DIR = BASE_DIR / 'logs'


def configure_logging():
    LOG_DIR.mkdir(exist_ok=True)
    logging.basicConfig(
        datefmt='%d.%m.%Y %H:%M:%S',
        format='"%(asctime)s - [%(levelname)s] - %(message)s"',
        level=logging.INFO,
        handlers=(
            RotatingFileHandler(
                LOG_DIR / 'tcp_asyncio_server_logs.log',
                maxBytes=10 ** 6,
                backupCount=5
            ),
            logging.StreamHandler()
        )
    )
