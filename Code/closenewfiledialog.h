#ifndef CLOSENEWFILEDIALOG_H
#define CLOSENEWFILEDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>

namespace Ui {
class closeNewFileDialog;
}

class closeNewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit closeNewFileDialog(QWidget *parent = nullptr, int i = -1, QString f = "", bool qC = false);
    QDialogButtonBox* getBBox();
    ~closeNewFileDialog();
    int ind = -1;
    QString fileLabel = "";

private slots:


private:
    Ui::closeNewFileDialog *ui;

};

#endif // CLOSENEWFILEDIALOG_H
