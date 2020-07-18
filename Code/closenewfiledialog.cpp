#include "closenewfiledialog.h"
#include "ui_closenewfiledialog.h"

closeNewFileDialog::closeNewFileDialog(QWidget *parent, int i) :
    QDialog(parent),
    ui(new Ui::closeNewFileDialog),
    ind(i)
{
    ui->setupUi(this);
}

QDialogButtonBox *closeNewFileDialog::getBBox()
{
    return ui->buttonBox;
}

closeNewFileDialog::~closeNewFileDialog()
{
    delete ui;
}

