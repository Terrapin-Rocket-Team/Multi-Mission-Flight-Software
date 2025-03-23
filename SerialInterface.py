import serial
import time

# Update this to match your Arduino's port
SERIAL_PORT = "COM51"  # Change to "/dev/ttyUSB0" or "/dev/ttyACM0" on Linux/macOS
BAUD_RATE = 9600


def copyFile(ser: serial.Serial, src, dest = ""):
    
    if(dest == ""):
        dest = src
    ser.write("cp " + src)
    file = open(dest,"w")
    time.sleep(0.1)
    while(ser.in_waiting > 0):
        file.write(ser.read(ser.in_waiting).decode())
    print(f"Copied \"{src}\" to \"dest\".")

def getlatestFiles(ser: serial.Serial):
    ser.write(("latest\n").encode())
    while(ser.in_waiting == 0):
        time.sleep(0.1)
    num = int(ser.read(ser.in_waiting).decode().strip())
    if(num < 1):
        print("No files found.")
        return
    
    copyFile(ser, f"{num}_FlightData.csv")
    copyFile(ser, f"{num}_Log.txt")
    copyFile(ser, f"{num}_PreFlightData.csv")


def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Wait for Arduino to initialize
        print(f"Connected to {SERIAL_PORT}")

        while True:
            # Send user input to Arduino
            cmd = input("Enter command: ")
            if(not cmd):
               continue
            args = cmd[cmd.find(" ") + 1:].split(" ")
            cmd = cmd[:cmd.find(" ")]
            if cmd.lower() in ["exit", "quit"]:
                print("Exiting...")
                break
            if(cmd.lower() == "latest"):
                getlatestFiles(ser)
            if(cmd.lower() == "cp" and args.count() == 1 and args[0].isdigit()):
                copyFile(ser, f"{args[0]}_FligthData.csv")
                copyFile(ser, f"{args[0]}_Log.txt")
                copyFile(ser, f"{args[0]}_PreFligthData.csv")
            else:
                ser.write((cmd + "\n").encode())

            # Read and print all incoming data
            time.sleep(0.1)  # Give Arduino time to respond
            while ser.in_waiting > 0:
                data = ser.read(ser.in_waiting).decode().strip()
                if data:
                    print("Arduino:", data)

    except serial.SerialException:
        print("Error: Could not open serial port. Check connection.")
    except KeyboardInterrupt:
        print("\nExiting...")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

if __name__ == "__main__":
    main()