#include "voiceplayer.h"
#include <QDebug>

VoicePlayer::VoicePlayer()
{

}

void VoicePlayer::voicePlay(QString file)
{
    QString cmd = QString("aplay %1%2").arg(VOICE_PATH).arg(file);
    qDebug()<<"[voicePlay]"<<cmd;

    if(player.state() == QProcess::Running)
    {
        player.close();
    }

    player.start(cmd);
}
