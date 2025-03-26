import argparse
from client import CryptoConnection
import time
from datetime import datetime

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Crypto data fetcher")
    parser.add_argument("symbol", type=str, help="Cryptocurrency symbol (e.g., ETH)")
    args = parser.parse_args()

    crypto_symbol = args.symbol

    connection = CryptoConnection()
    connection.connect()

    connection.start_streaming(crypto_symbol)

    for _ in range(10):
        time.sleep(1)
        data = connection.get_orderbook_data(crypto_symbol)

    time.sleep(5)
    connection.close()
