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
    if ports:
        return ports[0].device
    return None

ser = None

def writeSer(text):
    ser.write(("cmd/" + text + "\n").encode())

def copyFile(ser: serial.Serial, src, dest = ""):
    
    if(dest == ""):
        dest = src
    writeSer("cp " + src)
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
    writeSer("latest")
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
    
    writeSer("rm " + path)
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
    writeSer("clr")
    if(not ser.readline().decode().startswith("ok")):
        print("Arduino did not recognize \"clr\" command")
        return
    time.sleep(0.1)
    if(not ser.readline().decode().startswith("Removed")):
        print("Error")

def help():
    """
    Serial File Management Commands (Arduino-side):
    ----------------------------------------------
    Commands must be sent over an active serial connection.
    
    1. ls  
       - Lists all files stored on the Arduino.  

    2. cp <filename> or cp <source> <dest>
       - Downloads a file from the Arduino to the connected computer.  
 
    3. rm <filename>  
       - Deletes a file on the Arduino (confirmation required in Python). 

    4. latest  
       - Retrieves the most latest file from latest power cycle.

    5. clr  
       - Wipes ALL files on the Arduino (confirmation required in Python).  
       
    Note: 
    - The Python wrappers handle serial communication and user prompts.  
    - Use find_serial_port() to auto-detect the Arduino's serial port.  
    """
    print(help.__doc__)
    
def main():
    try: 
        port = find_serial_port()
        if not port:
            print("No serial port found.")
            return
        ser = serial.Serial(port, 9600, timeout=1)
        time.sleep(.5)  # Wait for Arduino to initialize
        print(f"Connected to {port}")

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
            elif(cmd.lower() == "help"):
                help()
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
                writeSer(cmd)

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