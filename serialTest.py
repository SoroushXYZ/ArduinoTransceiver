import serial
import serial.tools.list_ports
import struct

STRUCT_FORMAT = '<BcBBBhhBBB'  # Updated format

def list_serial_ports():
    ports = serial.tools.list_ports.comports()
    available_ports = [port.device for port in ports]
    print("Available serial ports:")
    for idx, port in enumerate(available_ports):
        print(f"{idx}: {port}")
    return available_ports

def main():
    ports = list_serial_ports()
    selected_index = int(input("Select your Arduino by number: "))
    ser = serial.Serial(ports[selected_index], 9600, timeout=2)

    while True:
        command = input("Enter command (e.g., C0) or 'exit' to quit: ").strip()
        if command.lower() == "exit":
            break

        ser.write(f"{command}\n".encode())  # Send command to Arduino
        data = ser.read(12)  # Read 12 bytes
        if len(data) == 12:
            print(f"Raw Data: {data.hex()}")
            parse_channel_config(data)
        else:
            print("Failed to parse struct. Data length may be incorrect.")

def parse_channel_config(data):
    try:
        values = struct.unpack(STRUCT_FORMAT, data)
        print(f"Version: {values[0]}, Device Type: {values[1].decode()}, Device ID: {values[2]}, Reverse: {values[3]}, Trim: {values[4]}")
        print(f"Analog Min: {values[5]}, Analog Max: {values[6]}, Min Endpoint: {values[7]}, Max Endpoint: {values[8]}, Center Point: {values[9]}")
    except struct.error as e:
        print(f"Error parsing struct: {e}")

if __name__ == "__main__":
    main()
