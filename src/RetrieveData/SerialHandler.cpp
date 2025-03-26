#include "SerialHandler.h"
using namespace mmfs;

void SerialHandler::handle()
{
    if (Serial.available())
    {
        int i = Serial.readBytesUntil('\n', cmdBuffer, sizeof(cmdBuffer));
        cmdBuffer[i] = '\0';
        char *cmd = strtok(cmdBuffer, " ");
        char *args = strtok(nullptr, "");
        if (strcmp(cmd, "ls") == 0)
        {
            Serial.println("ok ls");
            fetchList();
        }
        else if (strcmp(cmd, "clr") == 0)
        {
            Serial.println("ok clr");
            clearFiles();
        }
        else if (strcmp(cmd, "cp") == 0)
        {
            Serial.println("ok cp");
            copyFile(args);
        }

        else if (strcmp(cmd, "rm") == 0)
        {
            Serial.println("ok rm");
            removeFile(args);
        }
        else if (strcmp(cmd, "latest") == 0)
        {
            Serial.println("ok latest");
            latestFiles();
        }
        else
        {
            Serial.printf("no cmd: %s\n", cmd);
        }
    }
}

void SerialHandler::fetchList()
{
    getLogger().sd.ls(LS_DATE | LS_SIZE);
}

void SerialHandler::clearFiles()
{
    getLogger().sd.format();
    Serial.println("Removed All Files");
}

// expects "fileName_##.csv"
void SerialHandler::copyFile(char *args)
{
    SdFs &sd = getLogger().sd;
    if (!sd.exists(args))
    {
        Serial.print("File not found: ");
        Serial.println(args);
        return;
    }
    Serial.println("Sending...");
    FsFile f = sd.open(args, FILE_READ);
    char c;
    while (f.readBytes(&c, 1))
        Serial.write(c);
}

void SerialHandler::removeFile(char *args)
{

    if (!getLogger().sd.exists(args))
    {
        Serial.print("File not found: ");
        Serial.println(args);
        return;
    }
    getLogger().sd.remove(args);
    Serial.print("Removed ");
    Serial.println(args);
}

// prints out the # of the last power cycle's flight files.
void SerialHandler::latestFiles()
{
    Serial.println(getLogger().getFlightNum() - 1);
}

SerialHandler &mmfs::getSerialHandler()
{
        static SerialHandler SerialHandler;
        return SerialHandler;
}