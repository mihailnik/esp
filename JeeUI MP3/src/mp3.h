
#include <SD.h>
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
//#include "AudioOutputI2S.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2SNoDAC *out;
AudioFileSourceID3 *id3;


void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
    (void)cbData;
    Serial.printf("ID3 callback for: %s = '", type);

    if (isUnicode)
    {
        string += 2;
    }

    while (*string)
    {
        char a = *(string++);
        if (isUnicode)
        {
            string++;
        }
        Serial.printf("%c", a);
    }
    Serial.printf("'\n");
    Serial.flush();
}

void mp3_setup(String mp3_file)
{
    if(mp3_file.indexOf(".mp3") < 0)
        return;
    file = new AudioFileSourceSPIFFS(mp3_file.c_str());
    id3 = new AudioFileSourceID3(file);
    id3->RegisterMetadataCB(MDCallback, (void *)"ID3TAG");
    out = new AudioOutputI2SNoDAC();
    mp3 = new AudioGeneratorMP3();
    mp3->begin(id3, out);
}

void mp3_play()
{
    static bool mp3_stp = false;
    if(!mp3_stp)
    {
        mp3_setup("zkzk.mp3");
        mp3_stp = true;
    }
        
    if (mp3->isRunning()) {
        if (!mp3->loop())
        {
            mp3->stop();
        }
    }
}