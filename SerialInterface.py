import serial
import time
from serial.tools import list_ports

def find_serial_port():
    """
    Automatically finds and returns the first available serial port.
    Returns:
        str: The name of the serial port if found, otherwise None.
    """
    ports = list(list_ports.comports())
    return ports[0].device if ports else None

ser = None

def writeSer(text):
    ser.write(f"cmd/{text}\n".encode())

def copyFile(ser: serial.Serial, src, dest=""):
    if dest == "":
        dest = src
    writeSer(f"cp {src}")
    with open(dest, "wb") as file:
        time.sleep(1)
        if not ser.readline().decode().startswith("ok"):
            print("Arduino did not recognize \"cp\" command")
            return

        resp = ser.readline().decode().strip()
        if resp != "Sending...":
            print(resp)
            return

        timeout = 2  # Timeout in seconds after last received data
        start_time = time.time()
        while True:
            bytes_waiting = ser.in_waiting
            if bytes_waiting:
                data = ser.read(bytes_waiting)
                file.write(data)
                start_time = time.time()  # Reset timeout on data received
            else:
                # Check if timeout period has elapsed since last data
                if time.time() - start_time > timeout:
                    break
            time.sleep(0.1)  # Short sleep to prevent high CPU usage

    print(f"Copied \"{src}\" to \"{dest}\".")

def getlatestFiles(ser: serial.Serial):
    writeSer("latest")
    while ser.in_waiting == 0:
        time.sleep(0.1)
    if not ser.readline().decode().startswith("ok"):
        print("Arduino did not recognize \"latest\" command")
        return

    num = int(ser.readline().decode().strip())
    if num < 1:
        print("No files found.")
        return

    copyFile(ser, f"{num}_FlightData.csv")
    copyFile(ser, f"{num}_Log.txt")
    copyFile(ser, f"{num}_PreFlightData.csv")

def removeFile(ser: serial.Serial, path):
    if not path:
        print("No filepath provided.")
        return
    confirm = input(f"Remove \"{path}\"? [y/n]: ").lower()
    if confirm != 'y':
        print("Canceled.")
        return

    writeSer(f"rm {path}")
    if not ser.readline().decode().startswith("ok"):
        print("Arduino did not recognize \"rm\" command")
        return
    resp = ser.readline().decode().strip()
    if "Removed" not in resp:
        print(resp)
    else:
        print(f"Removed: \"{path}\"")

def clearFiles(ser: serial.Serial):
    confirm = input("Clear all files? [y/n]: ").lower()
    if confirm != 'y':
        print("Canceled.")
        return

    writeSer("clr")
    if not ser.readline().decode().startswith("ok"):
        print("Arduino did not recognize \"clr\" command")
        return
    resp = ser.readline().decode().strip()
    print(resp if "Removed" in resp else "Error")

def help():
    print("""
    Serial File Management Commands (Arduino-side):
    ----------------------------------------------
    Commands must be sent over an active serial connection.
    
    1. ls         - List all files on Arduino
    2. cp <file>  - Download a file (auto-saves to current directory)
    3. rm <file>  - Delete a file (confirmation required)
    4. latest     - Retrieve latest flight data files
    5. clr        - Wipe all files (confirmation required)
    6. help       - Show this help
    7. exit/quit  - Exit the program
    """)

def main():
    global ser
    try:
        port = "COM6"
        if not port:
            print("No serial port found.")
            return

        ser = serial.Serial(port, 9600, timeout=1)
        time.sleep(2)  # Allow Arduino to reset after connection
        print(f"Connected to {port}")

        while True:
            cmd = input("Enter command: ").strip()
            if not cmd:
                continue

            parts = cmd.split()
            action = parts[0].lower()
            args = parts[1:]

            if action in ("exit", "quit"):
                break
            elif action == "help":
                help()
            elif action == "latest":
                getlatestFiles(ser)
            elif action == "cp" and args:
                num = args[0]
                copyFile(ser, f"{num}_FlightData.csv")
                copyFile(ser, f"{num}_Log.txt")
                copyFile(ser, f"{num}_PreFlightData.csv")
            elif action == "rm" and args:
                removeFile(ser, args[0])
            elif action == "clr":
                clearFiles(ser)
            else:
                writeSer(cmd)
                time.sleep(0.1)
                while ser.in_waiting > 0:
                    print(ser.read(ser.in_waiting).decode())
                    time.sleep(0.1); # try and wait to make sure the arduino isnt going to send more.

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except KeyboardInterrupt:
        print("\nExiting...")
    finally:
        if ser and ser.is_open:
            ser.close()

if __name__ == "__main__":
    main()