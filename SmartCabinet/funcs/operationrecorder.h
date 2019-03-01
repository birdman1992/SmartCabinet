#ifndef OPERATIONRECORDER_H
#define OPERATIONRECORDER_H

#include <QObject>

class OperationRecorder : public QObject
{
    Q_OBJECT
public:
    explicit OperationRecorder(QObject *parent = 0);

signals:

public slots:
};

#endif // OPERATIONRECORDER_H