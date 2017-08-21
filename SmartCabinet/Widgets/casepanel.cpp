#include "casepanel.h"
#include "ui_casepanel.h"
#include <qdebug.h>
#include <QPainter>

CasePanel::CasePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CasePanel)
{
    ui->setupUi(this);
    font = new QFont("微软雅黑");
    font->setPixelSize(15);
    this->setFont(*font);
}

CasePanel::~CasePanel()
{
    delete font;
    delete ui;
}

void CasePanel::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
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

QFont CasePanel::caseFont()
{
    return *font;
}

int CasePanel::labWidth()
{
    return ui->left->width();
}

QString CasePanel::geteElidedText(QFont font, QString str, int MaxWidth)
{
    QFontMetrics fontWidth(font);
    int width = fontWidth.width(str);  //计算字符串宽度
    qDebug()<<"[geteElidedText]"<<str<<fontWidth.width(str)<<MaxWidth;  //qDebug获取"abcdefg..." 为60
    if(width>=MaxWidth)  //当字符串宽度大于最大宽度时进行转换
    {
        str = fontWidth.elidedText(str,Qt::ElideRight, ui->left->width());  //右部显示省略号
    }
    return str;   //返回处理后的字符串
}
