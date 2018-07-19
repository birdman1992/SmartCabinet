#ifndef KEYBOAD_H
#define KEYBOAD_H

#include <QWidget>
#include <QButtonGroup>

namespace Ui {
class Keyboad;
}

class Keyboad : public QWidget
{
    Q_OBJECT

public:
    explicit Keyboad(QWidget *parent = 0);
    ~Keyboad();

private:
    Ui::Keyboad *ui;
    QButtonGroup* btnGroup;

    void initKeys();

signals:
    void clicked(QString);

private slots:
    void keyClicked(QAbstractButton*);
};

#endif // KEYBOAD_H
