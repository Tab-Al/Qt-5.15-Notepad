#include "closenewfiledialog.h"
#include "ui_closenewfiledialog.h"
#include <QDialogButtonBox>
#include <QPushButton>

closeNewFileDialog::closeNewFileDialog(QWidget *parent, int i, QString f, bool qc) :
    QDialog(parent),
    ui(new Ui::closeNewFileDialog),
    ind(i),
    fileLabel(f)
{
    ui->setupUi(this);
    QString s = "File Name : ";
    s.append(fileLabel);
    ui->label_2->setText(s);

    if(qc == true)
    {
        ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    }
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

QDialogButtonBox *closeNewFileDialog::getBBox()
{
    return ui->buttonBox;
}

closeNewFileDialog::~closeNewFileDialog()
{
    delete ui;
}

