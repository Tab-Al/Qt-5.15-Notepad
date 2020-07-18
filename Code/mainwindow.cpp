#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextEdit>
#include "insidetab.h"
#include <bits/stdc++.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget((ui->tabWidget));

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tabSelected()
{
    int at = ui->tabWidget->currentIndex();
    currentFile = ui->tabWidget->tabText(at);
    qDebug() << "Active Tab Changed To : "<<currentFile;
}

QTextEdit* MainWindow::getCurrentTextEdit()
{
    QWidget *curTab = ui->tabWidget->currentWidget();

    // get the TextEdit Widget of current active tab
    QList<QTextEdit *> allTextEdits = curTab->findChildren<QTextEdit *>();
    // see if text edit widget found
    if (allTextEdits.count() != 1)
    {
        qDebug() << "Error in finding current tab's TextEdit";
        return NULL;
    }

    // return text edit widget
    return allTextEdits[0];
}

void MainWindow::on_actionNew_triggered()
{

    currentFile.clear();

    //keep track of total tabs opened
    noOfTabs++;

    // give new file temporary name
    QString newFileName = "New File (";
    newFileName.append(QString::number(noOfTabs));
    newFileName.append(")");

    // add new tab, set isNew to true
    QWidget *n = new InsideTab(ui->tabWidget, newFileName, "", true);
    ui->tabWidget->addTab(n, newFileName);
    currentFile = newFileName;

    // keep track of new tab
    pos[newFileName] = noOfTabs-1;

    // switch to new tab
    ui->tabWidget->setCurrentIndex(pos[newFileName]);
}


void MainWindow::on_actionOpen_triggered()
{
    //Get File Name to Open
    QString filter = "Text File (*.txt)";
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "C://", filter);

    // if same file exists open its tab
    if(pos.find(fileName) != pos.end())
    {
        ui->tabWidget->setCurrentIndex(pos[fileName]);
        return;
    }

    // open file
    QFile file(fileName);

    // show error if any
    if(!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "This file cannot be opened");
        return;
    }

    // read file contents
    currentFile = fileName;
    QTextStream in(&file);
    QString text = in.readAll();

    // add keep track of position of file
    noOfTabs++;
    pos[fileName] = noOfTabs-1;

    // add tab
    ui->tabWidget->addTab(new InsideTab(ui->tabWidget, fileName, text), fileName);

    // switch to tab
    ui->tabWidget->setCurrentIndex(pos[fileName]);
    file.close();
}



void MainWindow::on_actionSave_triggered()
{
    // get current active tab
    QWidget *curTab = ui->tabWidget->currentWidget();

    // get the TextEdit Widget of current active tab
    QList<QTextEdit *> allTextEdits = curTab->findChildren<QTextEdit *>();
    if (allTextEdits.count() != 1)
    {
        qDebug() << "Error in finding current tab's TextEdit";
        return;
    }


    QFile file;
    // check if new file
    auto t = dynamic_cast<InsideTab *>(curTab);
    if(t->getIsNew())
    {
        QString newFileName = QFileDialog::getSaveFileName(this, "Save As", "C://", "Text File (*.txt)");
        qDebug() << "Saving New File As : "<<newFileName;
        file.setFileName(newFileName);

        // update pos of tab with new file name
        pos[newFileName] = pos[currentFile];
        // remove older filename from map
        pos.remove(currentFile);

        // update tab's name
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), newFileName);
        // update tracker of current file
        currentFile = newFileName;
        t->setIsNew(false);
    }
    else
    {
        qDebug() << "Saving Current File : "<<currentFile;
        file.setFileName(currentFile);
    }


    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate| QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "This file cannot be opened");
    }

    QTextStream out(&file);

    QTextEdit *temp = allTextEdits[0];
    QString s = temp->toPlainText();

    out << s;
    file.close();
    qDebug() << "Save Successfull";
}

void MainWindow::on_actionSave_As_triggered()
{
    // get current active tab
    QWidget *curTab = ui->tabWidget->currentWidget();

    // get the TextEdit Widget of current active tab
    QList<QTextEdit *> allTextEdits = curTab->findChildren<QTextEdit *>();
    if (allTextEdits.count() != 1)
    {
        qDebug() << "Error in finding current tab's TextEdit";
        return;
    }

    QFile file;
    QString newFileName = QFileDialog::getSaveFileName(this, "Save As", "C://", "Text File (*.txt)");
    qDebug() << "Saving New File As : "<<newFileName;
    file.setFileName(newFileName);

    // update pos of tab with new file name
    pos[newFileName] = pos[currentFile];
    // remove older filename from map
    pos.remove(currentFile);

    // update tab's name
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), newFileName);
    // update tracker of current file
    currentFile = newFileName;

    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate| QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "This file cannot be opened");
    }

    QTextStream out(&file);

    QTextEdit *temp = allTextEdits[0];
    QString s = temp->toPlainText();

    out << s;
    file.close();
    qDebug() << "Save As Successfull";
}

void MainWindow::on_actionCopy_triggered()
{
    QTextEdit *current = getCurrentTextEdit();
    current->copy();
}

void MainWindow::on_actionCut_triggered()
{
    QTextEdit *current = getCurrentTextEdit();
    current->cut();
}


void MainWindow::on_actionPaste_triggered()
{
    QTextEdit *current = getCurrentTextEdit();
    current->paste();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}
