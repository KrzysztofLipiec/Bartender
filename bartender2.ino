#include <Regexp.h>
#include <SD.h>
#include <SPI.h>
#include "src/Config.cpp"
#include "src/engineController/engineController.cpp"

EngineController *engineController;

String input;
char buf[100];
// JsonObject &config;
int index = 0;
JsonObject sdConfig;
StaticJsonDocument<2048> doc;
int *array;

void setup()
{
    Serial.begin(115200);
    //LOAD CONFIg
    pinMode(4, OUTPUT);
    SD.begin(4);
    Serial.println("init");
    File file = SD.open("/c.txt");
    deserializeJson(doc, file);
    file.close();
    serializeJson(doc, Serial);
    Serial.print("\n");
    sdConfig = doc.as<JsonObject>();

    engineController = new EngineController(&sdConfig);
}

void loop()
{
    input = "";

    if (Serial.available() > 0)
    {
        input = Serial.readString().c_str();
        char *cstr = new char[input.length() + 1];
        strcpy(cstr, input.c_str());

        MatchState ms;
        ms.Target(cstr);
        if (ms.Match("calibrate") == REGEXP_MATCHED)
        {
            engineController->calibrate();
        }
        else if (ms.Match("M(%a?)(%d*)(%a?)(%d*)(%a?)(%d*)") == REGEXP_MATCHED)
        {
            ms.GetMatch(buf);
            char *commandChar;
            char *volumeChar;

            String command[10];
            String volume[10];
            int j;
            int i = 0;
            for (j = 0; j < ms.level; j++)
            {
                commandChar = ms.GetCapture(buf, j);
                command[i] = String(commandChar);
                j++;
                volumeChar = ms.GetCapture(buf, j);
                volume[i] = String(volumeChar);
                i++;
            }
            engineController->moveTo(command, volume, i);
        }
        else if (cstr[0] == 'D')
        {
            int size = 0;
            String sequence = input.substring(1);
            free(array);
            array = (int *)malloc((size) * sizeof(int));
            String part = "";
            for (int i = 0; i < sequence.length(); i++)
            {
                if (sequence.charAt(i) != ',')
                {
                    part += sequence.charAt(i);
                }
                else
                {
                    size++;
                    array = (int *)realloc(array, size * sizeof(int));
                    array[size - 1] = part.toInt();
                    part = "";
                }
            }
            size++;
            array = (int *)realloc(array, size * sizeof(int));
            array[size - 1] = part.toInt();
            part = "";

            engineController->prepareDrink(array, size);
            // for (int i = 0; i < size; i++)
            // {
            //     Serial.print(array[i]);
            //     Serial.print(',');
            // }
        }

        Serial.println("ok");
        delete[] cstr;
    }
}