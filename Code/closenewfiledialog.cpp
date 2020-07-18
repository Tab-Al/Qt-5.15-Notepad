#include "closenewfiledialog.h"
#include "ui_closenewfiledialog.h"

closeNewFileDialog::closeNewFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::closeNewFileDialog)
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

