import json
Import("env")

# Function to read the version from library.json
def get_version():
    with open("library.json", "r") as f:
        data = json.load(f)
        return data["version"]

# Call the Python script to get the version flag
version = get_version()
print(f"[MMFS CPP DEFINE] APP_VERSION: {version}")

# Append the version flag, making sure the version is passed as a string literal to C++
env.Append(CPPDEFINES=[f'APP_VERSION=\\"{version}\\"'])
