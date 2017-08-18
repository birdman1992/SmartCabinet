#ifndef VOICEPLAYER_H
#define VOICEPLAYER_H
#include <QProcess>
#include <QString>

#define VOICE_PATH "/home/config/"

#define VOICE_WELCOME "1.wav"
#define VOICE_SCAN "2.wav"
#define VOICE_CLOSE_DOOR "3.wav"
#define VOICE_WELCOME_USE "4.wav"
#define VOICE_ERROR "5.wav"

class VoicePlayer
{
public:
    VoicePlayer();
    void voicePlay(QString file);

private:
    QProcess player;
};

#endif // VOICEPLAYER_H
