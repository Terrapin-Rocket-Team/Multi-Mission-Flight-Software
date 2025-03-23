#include "SerialHandler.h"
using namespace mmfs;

void SerialHandler::handle()
{
    if (Serial.available())
    {
        int i = Serial.readBytesUntil('\n', cmdBuffer, sizeof(cmdBuffer));
        cmdBuffer[i] = '\0';
    }
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

void SerialHandler::fetchList()
{
    getLogger().sd.ls(LS_DATE | LS_SIZE);
}

void SerialHandler::clearFiles()
{
    Serial.println("Are you sure? [y/n]");
    double timeout = millis();
    while (!Serial.available() && millis() - timeout < 5000)
    {
        delay(50);
    }
    char a;
    Serial.readBytes(&a, 1);
    if (a == 'y' || a == 'Y')
    {
        getLogger().sd.format();
    }
    Serial.clear();
}

//expects "fileName_##.csv"
void SerialHandler::copyFile(char *args)
{
    SdFs &sd = getLogger().sd;
    if (!sd.exists(args)){
        Serial.println("File not found.");
        return;
    }
    FsFile f = sd.open(args, FILE_READ);
    char c;
    while(f.readBytes(&c, 1))
        Serial.write(c);
}

void SerialHandler::removeFile(char *args)
{

    if (!getLogger().sd.exists(args)){
        Serial.println("File not found.");
        return;
    }
    Serial.println("Are you sure? [y/n]");
    double timeout = millis();
    while (!Serial.available() && millis() - timeout < 5000)
    {
        delay(50);
    }
    char a;
    Serial.readBytes(&a, 1);
    if (a == 'y' || a == 'Y')
    {
        getLogger().sd.remove(args);
    }
    Serial.clear();
}

//prints out the # of the last power cycle's flight files.
void SerialHandler::latestFiles()
{
    Serial.println(getLogger().getFlightNum() - 1);
}