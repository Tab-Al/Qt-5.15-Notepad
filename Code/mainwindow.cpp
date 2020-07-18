#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextEdit>
#include "insidetab.h"
#include <bits/stdc++.h>
#include <QDebug>
#include "closenewfiledialog.h"
#include <QDialogButtonBox>
#include <QAbstractButton>



// constructor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget((ui->tabWidget));


    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeCurTab(int)));
}



// destructor
MainWindow::~MainWindow()
{
    delete ui;
}



// to keep track of current active tab
void MainWindow::tabSelected()
{
    int at = ui->tabWidget->currentIndex();
    currentFile = ui->tabWidget->tabText(at);
    qDebug() << "Active Tab Changed To : "<<currentFile;
}



// when a tab is closed
void MainWindow::closeCurTab(int index)
{
    // get info about the tab that is being closed
    QWidget *curTab = ui->tabWidget->widget(index);
    InsideTab *t = dynamic_cast<InsideTab *>(curTab);

    // if the closed tab is new, ask for saving
    if(t->getIsNew())
    {
        qDebug() << "Showing Dialog";
        // create dialog
        c = new closeNewFileDialog(this);
        connect(c->getBBox(), SIGNAL(clicked(QAbstractButton *)), this, SLOT(take_action(QAbstractButton *)));
        int x = c->exec();

        // Save or No pressed
        if(x == QDialog::Accepted)
               qDebug() << "Dialog Aceepted";
        // cancel pressed, return and dont close tab
        else if(x == QDialog::Rejected)
        {
            qDebug() << "Dialog Rejected";
            return;
        }
    }
    // if the closed tab file exists, check if changes made
    else
    {
        // open old file
        QString path = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
        QFile file(path);

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Error";
            return;
        }

        // read old file
        QTextStream in(&file);
        QString old = in.readAll();

        QTextEdit *temp = getCurrentTextEdit();
        QString news = temp->toPlainText();

        // compare old and new files
        int x = QString::compare(old, news);
        // if changes made
        if(x != 0)
        {
            qDebug() << "Showing Dialog";
            c = new closeNewFileDialog(this);
            connect(c->getBBox(), SIGNAL(clicked(QAbstractButton *)), this, SLOT(take_action(QAbstractButton *)));
            int x = c->exec();

            if(x == QDialog::Accepted)
                   qDebug() << "Dialog Accepted";
            else if(x == QDialog::Rejected)
            {
                qDebug() << "Dialog Rejected";
                return;
            }
        }
    }

    // update position map for all tabs still open
    for(int i = index+1; i < noOfTabs; i++)
    {
        QString filename = ui->tabWidget->tabText(i);
        pos[filename]--;
    }

    // remove closed tab from position map
    pos.remove(ui->tabWidget->tabText(index));
    // remove closed tab from tabWidget
    ui->tabWidget->removeTab(index);
    // decrease total no of tabs
    noOfTabs--;
}



// handle dialog box
void MainWindow::take_action(QAbstractButton *butPressed)
{
    QString s = butPressed->text();
    qDebug() << s << " pressed on Dialog";

    if(QString::compare(s, "Save") == 0)
    {
        on_actionSave_triggered();
        c->accept();
    }
    else if(QString::compare(s, "&No") == 0)
    {
        c->accept();
    }
    else if(QString::compare(s, "Cancel") == 0)
    {
        c->reject();
    }
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



// new action
void MainWindow::on_actionNew_triggered()
{
    currentFile.clear();

    //keep track of total tabs opened
    noOfTabs++;
    totalNewOpened++;

    // give new file temporary name
    QString newFileName = "New File (";
    newFileName.append(QString::number(totalNewOpened));
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



// open action
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



// save action
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
        t->setFilename(newFileName);
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



// Save As action
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


    auto t = dynamic_cast<InsideTab *>(curTab);
    t->setFilename(newFileName);

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



// copy action
void MainWindow::on_actionCopy_triggered()
{
    QTextEdit *current = getCurrentTextEdit();
    current->copy();
}



// cut action
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



// Exit action
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}



// undo action
void MainWindow::on_actionUndo_triggered()
{
    QTextEdit *current = getCurrentTextEdit();
    current->undo();
}
