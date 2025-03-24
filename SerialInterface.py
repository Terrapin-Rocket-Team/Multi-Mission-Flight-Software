import serial
import time

# Update this to match your Arduino's port
SERIAL_PORT = "COM7"  # Change to "/dev/ttyUSB0" or "/dev/ttyACM0" on Linux/macOS
BAUD_RATE = 9600


def copyFile(ser: serial.Serial, src, dest = ""):
    
    if(dest == ""):
        dest = src
    ser.write(("cp " + src + "\n").encode())
    file = open(dest,"wb")
    time.sleep(0.1)
    if(not ser.readline().decode().startswith("ok")):
        print("Arduino did not recognize \"cp\" command")
        return
    
    if((resp := ser.readline().decode().strip()) != "Sending..."):
        print(resp)
        return
    time.sleep(0.1)
    while(data := ser.read(ser.in_waiting)):
        file.write(data)
        time.sleep(0.1)
    print(f"Copied \"{src}\" to \"{dest}\".")

def getlatestFiles(ser: serial.Serial):
    ser.write(("latest\n").encode())
    while(ser.in_waiting == 0):
        time.sleep(0.1)
    if(not ser.readline().decode().startswith("ok")):
        print("Arduino did not recognize \"latest\" command")
        return
    
    num = int(ser.read(ser.in_waiting).decode().strip())
    if(num < 1):
        print("No files found.")
        return
    
    copyFile(ser, f"{num}_FlightData.csv")
    time.sleep(0.1)
    copyFile(ser, f"{num}_Log.txt")
    time.sleep(0.1)
    copyFile(ser, f"{num}_PreFlightData.csv")

def removeFile(ser: serial.Serial, path):
    if(path is None or path == ""):
        print("No filepath provided.")
        return
    confirm = input(f"Are you sure you want to remove \"{path}\"? [y/n]: ")
    if(confirm[0].lower() != "y"):
        print("Canceled.")
        return
    
    ser.write(("rm " + path + "\n").encode())
    if(not ser.readline().decode().startswith("ok")):
        print("Arduino did not recognize \"rm\" command")
        return
    time.sleep(0.1)
    if("Removed" not in (resp := ser.readline().decode().strip())):
        print(resp)
        return
    print(f"Removed : \"{path}\"")

def clearFiles(ser: serial.Serial):
    confirm = input("This will clear all files. Are you sure? [y/n]: ")
    if(confirm[0].lower() != "y"):
        print("Canceled.")
        return
    ser.write("clr\n".encode());
    if(not ser.readline().decode().startswith("ok")):
        print("Arduino did not recognize \"clr\" command")
        return
    time.sleep(0.1)
    if(not ser.readline().decode().startswith("Removed")):
        print("Error")
    
def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(.5)  # Wait for Arduino to initialize
        print(f"Connected to {SERIAL_PORT}")

        while True:
            # Send user input to Arduino
            cmd = input("Enter command: ")
            if(not cmd):
               continue
            parts = cmd.strip().split()
            cmd = parts[0]
            args = parts[1:]
            if cmd.lower() in ["exit", "quit"]:
                print("Exiting...")
                break
            if(cmd.lower() == "latest"):
                getlatestFiles(ser)
            elif(cmd.lower() == "cp" and args[0].isdigit()):
                    copyFile(ser, f"{args[0]}_FlightData.csv")
                    time.sleep(0.1)
                    copyFile(ser, f"{args[0]}_Log.txt")
                    time.sleep(0.1)
                    copyFile(ser, f"{args[0]}_PreFlightData.csv")
            elif(cmd.lower() == "rm"):
                removeFile(ser, args[0])
            elif(cmd.lower() == "clr"):
                clearFiles(ser)
            else:
                print("Sending Command: " + cmd)
                ser.write((cmd + "\n").encode())

            # Read and print all incoming data
            time.sleep(.1)  # Give Arduino time to respond
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