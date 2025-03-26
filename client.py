import socket
import struct
from dataclasses import dataclass

import time

@dataclass
class MarketData:
    best_bid_price: float
    best_bid_volume: float
    best_ask_price: float
    best_ask_volume: float

class CryptoConnection:
    def __init__(self, host="127.0.0.1", port=8080, timeout=5.0):
        self.host = host
        self.port = port
        self.timeout = timeout
        
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.settimeout(self.timeout)
        
        self.exchange_id = {"coinbase": 0}
        self.currency_id = {"BTC": 1, "ETH": 2, "XRP": 3, "SOL": 4, "DOGE": 5}

    def connect(self):
        """Connect to the server."""
        try:
            self.socket.connect((self.host, self.port))
            print(f"Connected to server at {self.host}:{self.port}")
        except ConnectionError as e:
            print(f"Failed to connect: {e}")
            raise

    def start_streaming(self, currency_name):
        """
        Tells the core component to start internally streaming and storing orderbook
        data for a specific currency.

        Args:
            currency_name (str): the name of the currency to start streaming (e.g., "BTC")
        """

        continue_channel = 1

        message = struct.pack("!II", continue_channel, self.currency_id[currency_name])
        self.socket.sendall(message)

    def get_orderbook_data(self, currency_name, num_levels=1):
        """
        Request and retrieve orderbook data for a specific currency.
        
        Args:
            currency_name (str): The name of the currency (e.g., "BTC").
            num_levels (int, optional): Number of levels to request. Defaults to 1.
            
        Returns:
            MarketData: Parsed market data containing best bid and ask information.
            None: If no data is received or there's an error.
        """

        try:
            continue_channel = 2
            message = struct.pack("!III", continue_channel, self.currency_id[currency_name], num_levels)
            self.socket.sendall(message)

            response = b''
            bytes_to_receive = 40
            while bytes_to_receive > 0:
                chunk = self.socket.recv(bytes_to_receive)
                if not chunk:
                    print("Server disconnected")
                    return None
                response += chunk
                bytes_to_receive -= len(chunk)

            currency_id = struct.unpack('>I', response[:4])[0]
            remaining_size = struct.unpack('>I', response[4:8])[0]

            best_bid_price = struct.unpack('>d', response[8:16])[0]
            best_bid_volume = struct.unpack('>d', response[16:24])[0]
            best_ask_price = struct.unpack('>d', response[24:32])[0]
            best_ask_volume = struct.unpack('>d', response[32:40])[0]
            
            market_data = MarketData(
                best_bid_price=best_bid_price,
                best_bid_volume=best_bid_volume,
                best_ask_price=best_ask_price,
                best_ask_volume=best_ask_volume
            )
            
            print("Best Bid and Best Ask Information:")
            print(f"Best Bid Price: {market_data.best_bid_price}")
            print(f"Best Bid Volume: {market_data.best_bid_volume}")
            print(f"Best Ask Price: {market_data.best_ask_price}")
            print(f"Best Ask Volume: {market_data.best_ask_volume}")
            print("------------")

            return market_data
        except socket.timeout:
            print(f"Timeout while requesting orderbook data for {currency_name}")
            return None
        except Exception as e:
            print(f"Error while requesting orderbook data for {currency_name}: {e}")
            return None
    
    def close(self):
        """
        Close the connection to the server.
        """
        try:
            continue_channel = 0
            message = struct.pack("!I", continue_channel)
            self.socket.sendall(message)
        except:
            pass
        
        if self.socket and hasattr(self, 'socket'):
            try:
                self.socket.close()
            except:
                pass

    def __del__(self):
        if self.socket:
            self.socket.close()
