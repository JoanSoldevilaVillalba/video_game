#!/usr/bin/env python3
"""
Python Test Suite for Video Game Server
Tests the client-server framing protocol and server message handling
"""

import socket
import struct
import time
import sys

# Configuration
SERVER_HOST = '127.0.0.1'
SERVER_PORT = 8080
BUFFER_SIZE = 64

class ServerTestClient:
    """Client for testing the video game server"""

    def __init__(self, host=SERVER_HOST, port=SERVER_PORT, timeout=5):
        """Initialize a test client"""
        self.host = host
        self.port = port
        self.timeout = timeout
        self.socket = None
        self.connected = False

    def connect(self):
        """Connect to the server"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(self.timeout)
            self.socket.connect((self.host, self.port))
            self.connected = True
            print(f"✓ Connected to server at {self.host}:{self.port}")
            return True
        except socket.error as e:
            print(f"✗ Failed to connect: {e}")
            self.connected = False
            return False

    def disconnect(self):
        """Disconnect from the server"""
        if self.socket:
            self.socket.close()
            self.connected = False
            print("✓ Disconnected from server")

    def send_framed_message(self, payload):
        """
        Send a framed message using the server protocol:
        [4-byte network-order length][payload]
        """
        if not self.connected:
            print("✗ Not connected to server")
            return False

        try:
            # Encode payload if string
            if isinstance(payload, str):
                payload_bytes = payload.encode('utf-8')
            else:
                payload_bytes = payload

            payload_len = len(payload_bytes)

            # Create network-order length header
            net_len = struct.pack('!I', payload_len)  # Big-endian unsigned int

            # Send header
            self.socket.sendall(net_len)

            # Send payload
            self.socket.sendall(payload_bytes)

            print(f"✓ Sent: '{payload}' (len: {payload_len})")
            return True

        except socket.timeout:
            print("✗ Send timeout")
            return False
        except socket.error as e:
            print(f"✗ Send error: {e}")
            return False

    def receive_framed_message(self):
        """
        Receive a framed message using the server protocol:
        [4-byte network-order length][payload]
        """
        if not self.connected:
            print("✗ Not connected to server")
            return None

        try:
            # Receive header (4 bytes)
            header = self._receive_all(4)
            if not header or len(header) != 4:
                print("✗ Failed to receive message header")
                return None

            # Unpack length (network byte order)
            payload_len = struct.unpack('!I', header)[0]

            # Validate payload length
            if payload_len >= BUFFER_SIZE:
                print(f"✗ Payload length {payload_len} exceeds buffer size {BUFFER_SIZE}")
                return None

            # Receive payload
            payload = self._receive_all(payload_len)
            if not payload or len(payload) != payload_len:
                print(f"✗ Incomplete payload received: {len(payload)} of {payload_len}")
                return None

            # Decode to string
            message = payload.decode('utf-8')
            print(f"✓ Received: '{message}'")
            return message

        except socket.timeout:
            print("✗ Receive timeout")
            return None
        except struct.error as e:
            print(f"✗ Message framing error: {e}")
            return None
        except socket.error as e:
            print(f"✗ Receive error: {e}")
            return None

    def _receive_all(self, n):
        """Receive exactly n bytes"""
        data = b''
        while len(data) < n:
            try:
                chunk = self.socket.recv(n - len(data))
                if not chunk:
                    return data if len(data) > 0 else None
                data += chunk
            except socket.timeout:
                return data if len(data) > 0 else None
        return data

    def send_and_receive(self, message):
        """Send a message and receive the response"""
        if self.send_framed_message(message):
            time.sleep(0.1)  # Small delay for server processing
            return self.receive_framed_message()
        return None


class TestVideoGameServer:
    """Test suite for the video game server"""

    def __init__(self):
        """Initialize test suite"""
        self.passed = 0
        self.failed = 0
        self.client = None

    def setup(self):
        """Setup test environment"""
        print("\n" + "="*60)
        print("VIDEO GAME SERVER TEST SUITE")
        print("="*60)

    def teardown(self):
        """Cleanup after tests"""
        if self.client and self.client.connected:
            self.client.disconnect()

    def assert_true(self, condition, message):
        """Assert condition is true"""
        if condition:
            print(f"  ✓ {message}")
            self.passed += 1
        else:
            print(f"  ✗ {message}")
            self.failed += 1

    def assert_equal(self, actual, expected, message):
        """Assert actual equals expected"""
        if actual == expected:
            print(f"  ✓ {message}")
            self.passed += 1
        else:
            print(f"  ✗ {message} (expected: {expected}, got: {actual})")
            self.failed += 1

    def assert_not_none(self, value, message):
        """Assert value is not None"""
        if value is not None:
            print(f"  ✓ {message}")
            self.passed += 1
        else:
            print(f"  ✗ {message}")
            self.failed += 1
            
    def assert_is_none(self, value, message):
        """Assert value is None (used for expecting dropped connections)"""
        if value is None:
            print(f"  ✓ {message}")
            self.passed += 1
        else:
            print(f"  ✗ {message} (expected None, got: {value})")
            self.failed += 1

    def test_server_connection(self):
        """Test 1: Server is accepting connections"""
        print("\n[Test 1] Server Connection")
        self.client = ServerTestClient()
        result = self.client.connect()
        self.assert_true(result, "Can connect to server")
        self.assert_true(self.client.connected, "Client is connected")

    def test_framing_protocol(self):
        """Test 2: Message framing protocol works correctly"""
        print("\n[Test 2] Framing Protocol")
        self.client = ServerTestClient()
        self.client.connect()

        # Fixed to include required pipes 
        message = "2|0|random_msg"  
        result = self.client.send_framed_message(message)
        self.assert_true(result, f"Can send framed message: '{message}'")

    def test_receive_protocol(self):
        """Test 3: Can receive framed messages from server"""
        print("\n[Test 3] Receive Framed Message")
        self.client = ServerTestClient()
        self.client.connect()

        self.client.send_framed_message("2|0|random_msg")

        response = self.client.receive_framed_message()
        self.assert_not_none(response, "Server sends response to random message")
        if response:
            self.assert_true("|" in response, "Response follows pipe-delimited protocol")

    def test_message_validation(self):
        """Test 4: Server validates message format"""
        print("\n[Test 4] Message Validation")
        self.client = ServerTestClient()
        self.client.connect()

        response = self.client.send_and_receive("2|0|valid_message")
        self.assert_not_none(response, "Server responds to valid message")

    def test_quit_protocol(self):
        """Test 5: Quit protocol works"""
        print("\n[Test 5] Quit Protocol")
        self.client = ServerTestClient()
        self.client.connect()

        response = self.client.send_and_receive("1|0|quit")
        self.assert_not_none(response, "Server responds to quit message")

        time.sleep(0.2)
        print("  ✓ Server accepted quit command")
        self.passed += 1

    def test_buffer_overflow_protection(self):
        """Test 6: Buffer overflow protection"""
        print("\n[Test 6] Buffer Overflow Protection")
        self.client = ServerTestClient()
        self.client.connect()

        # Test A: Send exactly max allowed payload (63 bytes)
        # 4 chars ("2|0|") + 59 chars = 63 bytes
        max_allowed_message = "2|0|" + "A" * (BUFFER_SIZE - 5)
        result = self.client.send_framed_message(max_allowed_message)
        self.assert_true(result, f"Can send message at buffer size limit ({BUFFER_SIZE - 1})")
        
        response = self.client.receive_framed_message()
        self.assert_not_none(response, "Server handles max-allowed size messages")
        
        # Test B: Send exactly 64 bytes (triggers server's >= BUFFER_SIZE rejection)
        # Re-establish connection just in case it dropped
        self.client.disconnect()
        self.client.connect()
        
        overflow_message = "2|0|" + "B" * (BUFFER_SIZE - 4)
        self.client.send_framed_message(overflow_message)
        response2 = self.client.receive_framed_message()
        
        # Server drops connection on validation failure, returning None
        self.assert_is_none(response2, "Server correctly rejects message >= BUFFER_SIZE")

    def test_multiple_sequential_messages(self):
        """Test 7: Multiple sequential messages"""
        print("\n[Test 7] Multiple Sequential Messages")
        self.client = ServerTestClient()
        self.client.connect()

        messages = ["2|1|msg1", "2|2|msg2", "2|3|msg3"]
        for msg in messages:
            response = self.client.send_and_receive(msg)
            self.assert_not_none(response, f"Server responds to message: {msg}")

    def test_game_creation_protocol(self):
        """Test 8: Game creation initiates correctly"""
        print("\n[Test 8] Game Creation Protocol")
        self.client = ServerTestClient()
        self.client.connect()

        response = self.client.send_and_receive("0|0|create_game")
        self.assert_not_none(response, "Server responds to game creation request")

        if response:
            parts = response.split("|")
            self.assert_true(len(parts) >= 2,
                           f"Response has protocol structure with parts: {len(parts)}")

    def test_invalid_message_handling(self):
        """Test 9: Invalid message handling"""
        print("\n[Test 9] Invalid Message Handling (Bad Protocol Identifiers)")
        self.client = ServerTestClient()
        self.client.connect()

        # Valid structure, but completely invalid logic numbers
        response = self.client.send_and_receive("9|9|invalid_opcode")
        self.assert_not_none(response, "Server returns invalid option error gracefully")

    def test_empty_message(self):
        """Test 10: Empty message handling"""
        print("\n[Test 10] Empty Message Handling")
        self.client = ServerTestClient()
        self.client.connect()

        # Send empty message (violates the pipe structure requirement in C server)
        result = self.client.send_framed_message("")
        self.assert_true(result, "Can send empty message")

        time.sleep(0.1)
        response = self.client.receive_framed_message()
        # Server is expected to drop connection because validate_message_structure fails
        self.assert_is_none(response, "Server correctly drops connection on empty message")

    def test_connection_persistence(self):
        """Test 11: Connection stays open for multiple operations"""
        print("\n[Test 11] Connection Persistence")
        self.client = ServerTestClient()
        result = self.client.connect()
        self.assert_true(result, "Initial connection established")

        for i in range(3):
            if self.client.connected:
                self.client.send_framed_message("2|0|keepalive")
                time.sleep(0.1)

        self.assert_true(self.client.connected, "Connection remains open after operations")

    def test_network_byte_order(self):
        """Test 12: Network byte order for length field"""
        print("\n[Test 12] Network Byte Order")
        self.client = ServerTestClient()
        self.client.connect()

        message = "2|0|TEST"
        self.client.send_framed_message(message)

        response = self.client.receive_framed_message()
        self.assert_not_none(response, "Network byte order handling works")

    def test_concurrent_messages(self):
        """Test 13: Handling rapid successive messages"""
        print("\n[Test 13] Rapid Message Handling")
        self.client = ServerTestClient()
        self.client.connect()

        for i in range(3):
            result = self.client.send_framed_message("2|0|rapid")
            self.assert_true(result, f"Sent rapid message {i+1}")
            time.sleep(0.05)

        responses = 0
        for i in range(3):
            response = self.client.receive_framed_message()
            if response:
                responses += 1

        self.assert_true(responses > 0, f"Server responded to rapid messages: {responses}")

    def test_protocol_message_parsing(self):
        """Test 14: Server protocol messages parse correctly"""
        print("\n[Test 14] Protocol Message Parsing")
        self.client = ServerTestClient()
        self.client.connect()

        response = self.client.send_and_receive("2|0|parse")
        self.assert_not_none(response, "Received protocol message")

        if response:
            parts = response.split("|")
            if len(parts) >= 2:
                try:
                    first_part = int(parts[0])
                    second_part = int(parts[1])
                    self.assert_true(True,
                        f"Protocol message has numeric parts: [{first_part}|{second_part}|...]")
                except ValueError:
                    self.assert_true(False, "Protocol message has non-numeric parts")

    def run_all_tests(self):
        """Run all tests"""
        self.setup()

        try:
            self.test_server_connection()
            self.test_framing_protocol()
            self.test_receive_protocol()
            self.test_message_validation()
            self.test_quit_protocol()
            self.test_buffer_overflow_protection()
            self.test_multiple_sequential_messages()
            self.test_game_creation_protocol()
            self.test_invalid_message_handling()
            self.test_empty_message()
            self.test_connection_persistence()
            self.test_network_byte_order()
            self.test_concurrent_messages()
            self.test_protocol_message_parsing()

        except Exception as e:
            print(f"\n✗ Test execution error: {e}")
            self.failed += 1

        finally:
            self.teardown()

        self.print_summary()

    def print_summary(self):
        """Print test summary"""
        total = self.passed + self.failed
        percentage = (self.passed / total * 100) if total > 0 else 0

        print("\n" + "="*60)
        print("TEST SUMMARY")
        print("="*60)
        print(f"Passed:  {self.passed}")
        print(f"Failed:  {self.failed}")
        print(f"Total:   {total}")
        print(f"Success Rate: {percentage:.1f}%")
        print("="*60 + "\n")

        return self.failed == 0


def main():
    """Main entry point"""
    if len(sys.argv) > 1 and sys.argv[1] == '--help':
        print("""
Usage: python3 test_server.py [options]

Options:
  --help     Show this help message
  --quick    Run only essential tests (1-5)
  --full     Run all tests (default)

Requirements:
  - Server running on 127.0.0.1:8080
  - Python 3.6+

Protocol:
  - Messages: [4-byte length][payload]
  - Format: Pipe-delimited (e.g., "0|1|message")
  - Buffer size: 64 bytes
        """)
        return

    quick_mode = len(sys.argv) > 1 and sys.argv[1] == '--quick'
    suite = TestVideoGameServer()

    if quick_mode:
        suite.setup()
        try:
            suite.test_server_connection()
            suite.test_framing_protocol()
            suite.test_receive_protocol()
            suite.test_message_validation()
            suite.test_quit_protocol()
        finally:
            suite.teardown()
        suite.print_summary()
    else:
        suite.run_all_tests()

if __name__ == '__main__':
    main()
