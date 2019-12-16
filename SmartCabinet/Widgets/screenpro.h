#ifndef SCREENPRO_H
#define SCREENPRO_H

#include <QWidget>

namespace Ui {
class ScreenPro;
}

class ScreenPro : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenPro(QWidget *parent = 0);
    ~ScreenPro();

public slots:
    void updateProState(bool);
private:
    Ui::ScreenPro *ui;

};

#endif // SCREENPRO_H
