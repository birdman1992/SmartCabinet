#include "casepanel.h"
#include "ui_casepanel.h"
#include <qdebug.h>
#include <QPainter>
#include <QRegExp>

CasePanel::CasePanel(bool doubleCol, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CasePanel)
{
    ui->setupUi(this);
    isSpec = false;
    font = new QFont("WenQuanYi Micro Hei Mono");
    font->setPixelSize(12);//另外需要修改cabinet.ui style sheet
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
        if(showDoubleCol)
            ui->right->setStyleSheet("image: url(:/image/image/icon_check.png);image-position:top right");
        else
            ui->left->setStyleSheet("image: url(:/image/image/icon_check.png);image-position:top right");
    }
    else
    {
        if(isSpec)
        {
            this->setStyleSheet(cellStyle(QColor(36, 221, 59)));
            return;
        }
        if(showDoubleCol)
            ui->right->setStyleSheet("");
        else
            ui->left->setStyleSheet("");
    }
}

QString CasePanel::cellStyle(QColor rgb)
{
    QString ret = QString("color:rgb(255,255,255);\
                          background-color: rgb(%1, %2, %3);\
            margin-top:2px;\
            margin-bottom:2px;").arg(rgb.red()).arg(rgb.green()).arg(rgb.blue());
            return ret;
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
//    qDebug()<<"resize"<<ui->left->width();
    updatePanel();
}

void CasePanel::enterEvent(QEvent *e)
{
//    qDebug()<<"enter panel";
    QWidget::enterEvent(e);
}

void CasePanel::leaveEvent(QEvent *e)
{
//    qDebug()<<"leave panel";
    QWidget::leaveEvent(e);
}

bool CasePanel::event(QEvent *e)
{
//    qDebug()<<e->type()<<this;
//    if(e->type() == QEvent::Enter)
//        qDebug()<<"enter"<<this;
//    if(e->type() == QEvent::Leave)
//        qDebug()<<"leave"<<this;
    return QWidget::event(e);
}

void CasePanel::setText(QStringList text)
{
    if(isSpec)
        return;

    QString left;
    QString right;
    int maxLine = getMaxLine();
    cur_show = text;
//    qDebug()<<"[setText]"<<text<<text.count();

    if(showDoubleCol)
    {
        if(cur_show.count() > maxLine)
            ui->right->show();
        else
            ui->right->hide();
        for(int i=0; i<cur_show.count(); i++)
        {
            if(i<maxLine)
            {
                QString str = getShowStr(cur_show.at(i));
                if(str.isEmpty())
                    continue;
                left += str;
                if(i<(maxLine-1)&&(i<cur_show.count()-1))
                    left += "\n";
            }
            else if(i<(2*maxLine))
            {
                QString str = getShowStr(cur_show.at(i));
                if(str.isEmpty())
                    continue;
                right += str;
                if(i<(2*maxLine-1)&&(i<cur_show.count()-1))
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
        for(int i=0; i<cur_show.count(); i++)
        {
            if(i<maxLine)
            {
                QString str = getShowStr(cur_show.at(i));
                if(str.isEmpty())
                    continue;
                left += str;
                if((i<maxLine-1) && (i<cur_show.count()-1))
                    left += "\n";
            }
            else
                break;
        }
        ui->left->setText(left);
    }
    this->setToolTip(text.join("\n"));
}

void CasePanel::setText(QList<Goods *> list)
{
    if(isSpec)
        return;

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
        return (getMaxLine())*2;
    }
    else
    {
        return getMaxLine();
    }
}

void CasePanel::setSpec(bool spec)
{
    isSpec = spec;
    showDoubleCol = false;
    if(spec)
    {
        ui->left->setText("护士长储物柜");
        ui->left->show();
        ui->right->hide();
        this->setStyleSheet(cellStyle(QColor(36, 221, 59)));
    }
    else
    {
        font = new QFont("WenQuanYi Micro Hei Mono");
        font->setPixelSize(12);//另外需要修改cabinet.ui style sheet
        this->setFont(*font);
        this->setStyleSheet(cellStyle(QColor(36, 221, 159)));
    }
}

bool CasePanel::isSpecialCase()
{
    return isSpec;
}

QString CasePanel::geteElidedText(QFont _font, QString str, int MaxWidth)
{
    QFontMetrics fontWidth(_font);
    int width = fontWidth.width(str);  //计算字符串宽度
    qDebug()<<"[geteElidedText]"<<str<<fontWidth.width(str)<<MaxWidth;  //qDebug获取"abcdefg..." 为60
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
    if(isSpec)
        return;
    setText(cur_show);
    return;
//    qDebug()<<"[updatePanel]";
    QString left;
    QString right;
    int i = 0;
    int maxLine = getMaxLine();
//    qDebug()<<"[getMaxLine]"<<maxLine;

    if(showDoubleCol)
    {
        if(list_show.count() > maxLine)
            ui->right->show();
        else
            ui->right->hide();
        for(i=0; i<list_show.count(); i++)
        {
            if(i<maxLine)
            {
                QString str = getShowStr(list_show.at(i));
                if(str.isEmpty())
                    continue;
                left += str;
                if(i<(maxLine-1)&&(i<list_show.count()-1))
                    left += "\n";
            }
            else if(i<(2*maxLine))
            {
                QString str = getShowStr(list_show.at(i));
                if(str.isEmpty())
                    continue;
                right += str;
                if(i<(2*maxLine-1)&&(i<list_show.count()-1))
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
                QString str = getShowStr(list_show.at(i));
                if(str.isEmpty())
                    continue;
                left += str;
                if((i<maxLine-1) && (i<list_show.count()-1))
                    left += "\n";
            }
            else
                break;
        }
        ui->left->setText(left);
    }
}

QString CasePanel::getShowStr(Goods *info)
{
//    if(info->num == 0)
//        return QString();
    QString str = info->name;
    QString strTail = QString("[%2]×%1").arg(info->num).arg(info->size);

    if(!info->abbName.isEmpty())
        str = info->abbName;

    int maxWidth = this->width();
    if(showDoubleCol)
    {
        maxWidth = maxWidth/2;
    }
    maxWidth = maxWidth - getStringWidth(strTail) - 10;
    qDebug()<<"[getshow str]"<<showDoubleCol<<ui->left->width()<<ui->right->width()<<this->width()<<getStringWidth(strTail);

    str = geteElidedText(*font, str, maxWidth);
    str += strTail;

    return str;
}

//缩短形如  [物品名]×[数量]形式的字符串
QString CasePanel::getShowStr(QString goodsStr)
{
    QRegExp rex;
    rex.setPattern("(.*)(×.*)");
    if(goodsStr.indexOf(rex) == -1)
        return goodsStr;

    int maxWidth = this->width();
    if(ui->right->isVisible())
    {
        maxWidth = maxWidth/2;
    }
    maxWidth = maxWidth - getStringWidth(rex.cap(2)) - 10;
//    qDebug()<<"[getshow str]"<<showDoubleCol<<ui->left->width()<<ui->right->width()<<this->width()<<getStringWidth(strTail);

    QString ret = geteElidedText(*font, rex.cap(1), maxWidth);
    ret += rex.cap(2);
    return ret;
}

int CasePanel::getMaxLine()
{
    QFontMetrics fontWidth(*font);
    return this->height()/(fontWidth.height());
}
