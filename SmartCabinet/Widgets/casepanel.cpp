#include "casepanel.h"
#include "ui_casepanel.h"
#include <qdebug.h>
#include <QPainter>

CasePanel::CasePanel(bool doubleCol, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CasePanel)
{
    ui->setupUi(this);
    font = new QFont("微软雅黑");
    font->setPixelSize(15);
    this->setFont(*font);
    list_show.clear();
    showDoubleCol = doubleCol;
}

CasePanel::~CasePanel()
{
    delete font;
    delete ui;
}

void CasePanel::setCheckState(bool checked)
{
    if(checked)
    {
        ui->right->setStyleSheet("image: url(:/image/image/icon_check.png);\
                                 image-position:top right");
    }
    else
    {
        ui->right->setStyleSheet("");
    }
}

void CasePanel::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CasePanel::resizeEvent(QResizeEvent *)
{
    updatePanel();
}

void CasePanel::setText(QStringList text)
{
    QString left;
    QString right;
    qDebug()<<"[setText]"<<text<<text.count();

    int i = 0;
    for(i=0; i<text.count(); i++)
    {
        if(!(i%2))
            left += text.at(i) + "\n";
        else
            right += text.at(i) + "\n";
    }

    ui->left->setText(left);
    ui->right->setText(right);
}

void CasePanel::setText(QList<GoodsInfo *> list)
{
    list_show = list;
    updatePanel();
}

QFont CasePanel::caseFont()
{
    return *font;
}

int CasePanel::labWidth()
{
    return ui->left->width();
}

int CasePanel::maxShowNum()
{
    if(showDoubleCol)
    {
        return (getMaxLine()-1)*2;
    }
    else
    {
        return getMaxLine()-1;
    }
}

QString CasePanel::geteElidedText(QFont _font, QString str, int MaxWidth)
{
    QFontMetrics fontWidth(_font);
    int width = fontWidth.width(str);  //计算字符串宽度
//    qDebug()<<"[geteElidedText]"<<str<<fontWidth.width(str)<<MaxWidth;  //qDebug获取"abcdefg..." 为60
    if(width>=MaxWidth)  //当字符串宽度大于最大宽度时进行转换
    {
        str = fontWidth.elidedText(str,Qt::ElideRight, MaxWidth);  //右部显示省略号
    }
    return str;   //返回处理后的字符串
}

int CasePanel::getStringWidth(QString str)
{
    QFontMetrics fontWidth(*font);
    return fontWidth.width(str);
}

void CasePanel::updatePanel()
{
//    qDebug()<<"[updatePanel]";
    QString left;
    QString right;
    int i = 0;
    int maxLine = getMaxLine()-1;

    if(showDoubleCol)
    {
        ui->right->show();
        for(i=0; i<list_show.count(); i++)
        {
            if(i<maxLine)
            {
                left += getShowStr(list_show.at(i));
                if(i<maxLine-1)
                    left += "\n";
            }
            else if(i<(2*maxLine))
            {
                right += getShowStr(list_show.at(i));
                if(i<(2*maxLine-1))
                    right += "\n";
            }
            else
                break;
        }
        ui->left->setText(left);
        ui->right->setText(right);
    }
    else
    {
        ui->right->hide();
        for(i=0; i<list_show.count(); i++)
        {
            if(i<maxLine)
            {
                left += getShowStr(list_show.at(i));
                if(i<maxLine-1)
                    left += "\n";
            }
            else
                break;
        }
        ui->left->setText(left);
    }
}

QString CasePanel::getShowStr(GoodsInfo *info)
{
    QString str = info->name;
    QString strTail = QString("×%1").arg(info->num);

    if(!info->abbName.isEmpty())
        str = info->abbName;

    str = geteElidedText(*font, str, ui->left->width() - getStringWidth(strTail) - 10);
    str += strTail;

    return str;
}

int CasePanel::getMaxLine()
{
    QFontMetrics fontWidth(*font);
    return ui->left->height()/fontWidth.height();
}
