#include "insidetab.h"
#include "ui_insidetab.h"

InsideTab::InsideTab(QWidget *parent, QString filename, QString content, bool ifNew)
    : QWidget(parent),
      ui(new Ui::InsideTab),
      filename(filename),
      isNew(ifNew)
{
    ui->setupUi(this);
    ui->textEdit->setText(content);
}

InsideTab::~InsideTab()
{
    delete ui;
}

bool InsideTab::getIsNew() const
{
    return isNew;
}

void InsideTab::setIsNew(bool value)
{
    isNew = value;
}

void InsideTab::setFilename(const QString &value)
{
    filename = value;
}
