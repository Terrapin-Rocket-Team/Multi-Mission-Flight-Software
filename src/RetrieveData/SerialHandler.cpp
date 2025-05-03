#include "SerialHandler.h"
#include "Events/Event.h"
using namespace mmfs;

void SerialHandler::handle()
{
    if (Serial.available())
    {
        int i = Serial.readBytesUntil('\n', line, sizeof(line));
        if (strncmp("cmd/", line, 4) == 0)
        {
            line[i] = '\0';
            char *cmd = strtok(line+4, " ");
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
        else{
            getEventManager().invoke(Event("SERIAL_LINE"_i));
        }
    }
}

void SerialHandler::fetchList()
{
    getLogger().backend->ls(LS_DATE | LS_SIZE);
}

void SerialHandler::clearFiles()
{
    getLogger().backend->format();
    Serial.println("Removed All Files");
}

// expects "fileName_##.csv"
void SerialHandler::copyFile(char *args)
{
    LoggingBackend *backend = getLogger().backend;
    if (!backend->exists(args))
    {
        Serial.print("File not found: ");
        Serial.println(args);
        return;
    }
    Serial.println("Sending...");
    LoggingBackendFile *f = backend->open(args);
    f->seek(0);
    char c;
    while (f->readBytes(&c, 1))
        Serial.write(c);
    delete f;
}

void SerialHandler::removeFile(char *args)
{

    if (!getLogger().backend->exists(args))
    {
        Serial.print("File not found: ");
        Serial.println(args);
        return;
    }
    getLogger().backend->remove(args);
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

const char *SerialHandler::getLastLine() const{
    return line;
}