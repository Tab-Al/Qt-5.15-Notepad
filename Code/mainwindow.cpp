#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextEdit>
#include "insidetab.h"
#include <bits/stdc++.h>
#include <QDebug>
#include "closenewfiledialog.h"
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QTimer>


// constructor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget((ui->tabWidget));
    ui->statusBar->showMessage("To Open File (Ctrl + O)  |  To Create File (Ctrl + N)");

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeCurTab(int)));
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quitting()));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(changeMsg()));
    timer->start(5000);
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
void MainWindow::closeCurTab(int index, bool quitting)
{
    qDebug() << "Trying to close : " << ui->tabWidget->tabText(index);
    // get info about the tab that is being closed
    QWidget *curTab = ui->tabWidget->widget(index);
    InsideTab *t = dynamic_cast<InsideTab *>(curTab);

    // if the closed tab is new, ask for saving
    if(t->getIsNew())
    {
        qDebug() << "Showing Dialog";
        // create dialog
        // if quitting then disable Cancel Button
        if(quitting == true)
            c = new closeNewFileDialog(this, index, ui->tabWidget->tabText(index), true);
        else
            c = new closeNewFileDialog(this, index, ui->tabWidget->tabText(index), false);
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
            if(quitting == true)
                c = new closeNewFileDialog(this, index, ui->tabWidget->tabText(index), true);
            else
                c = new closeNewFileDialog(this, index, ui->tabWidget->tabText(index), false);

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

    qDebug() << "Closed Successfully";
}



// handle dialog box
void MainWindow::take_action(QAbstractButton *butPressed)
{
    QString s = butPressed->text();
    qDebug() << s << " pressed on Dialog";

    if(QString::compare(s, "Save") == 0)
    {
        saveOnClose(c->ind);
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



// close All before quitting
void MainWindow::quitting()
{
    while(noOfTabs > 0)
    {

        closeCurTab(0, true);
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

    if(fileName.isNull())
    {
        qDebug() << "No file Selected ";
        return;
    }

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
        return;
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

    if(newFileName.isNull())
    {
        qDebug() << "No file selected ";
        return;
    }

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
        return;
    }

    QTextStream out(&file);

    QTextEdit *temp = allTextEdits[0];
    QString s = temp->toPlainText();

    out << s;
    file.close();
    qDebug() << "Save As Successfull";
}




// save on close action
void MainWindow::saveOnClose(int index)
{
    // get current active tab
    QWidget *curTab = ui->tabWidget->widget(index);

    // get the TextEdit Widget of current active tab
    QList<QTextEdit *> allTextEdits = curTab->findChildren<QTextEdit *>();
    if (allTextEdits.count() != 1)
    {
        qDebug() << "Error in finding current tab's TextEdit";
        return;
    }

    QString s = ui->tabWidget->tabText(index);
    QFile file;
    // check if new file
    auto t = dynamic_cast<InsideTab *>(curTab);
    if(t->getIsNew())
    {
        QString newFileName = QFileDialog::getSaveFileName(this, "Save As", "C://", "Text File (*.txt)");

        if(newFileName.isNull())
        {
            qDebug() << "No file selected";
            return;
        }

        qDebug() << "Saving New File As : "<<newFileName;
        file.setFileName(newFileName);

        // update pos of tab with new file name
        pos[newFileName] = pos[s];
        // remove older filename from map
        pos.remove(s);
    }
    else
    {
        qDebug() << "Saving File : "<<s;
        file.setFileName(s);
    }

    // open file
    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate| QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "This file cannot be opened");
        return;
    }

    // write to file
    QTextStream out(&file);
    QTextEdit *temp = allTextEdits[0];
    QString ss = temp->toPlainText();

    out << ss;
    file.close();
    qDebug() << "Save Successfull";
}


// copy action
void MainWindow::on_actionCopy_triggered()
{
    QTextEdit *current = getCurrentTextEdit();
    if(current)
        current->copy();
}



// cut action
void MainWindow::on_actionCut_triggered()
{
    QTextEdit *current = getCurrentTextEdit();
    if(current)
        current->cut();
}



void MainWindow::on_actionPaste_triggered()
{
    QTextEdit *current = getCurrentTextEdit();
    if(current)
        current->paste();
}



// Exit action
void MainWindow::on_actionExit_triggered()
{
    quitting();
    QApplication::quit();
}



// undo action
void MainWindow::on_actionUndo_triggered()
{
    QTextEdit *current = getCurrentTextEdit();
    if(current)
        current->undo();
}



// close current tab action
void MainWindow::on_actionClose_Current_Tab_triggered()
{
    closeCurTab(ui->tabWidget->currentIndex());
}



// change status bar msg
void MainWindow::changeMsg()
{
    QString s1("To Open File (Ctrl + O)  |  To Create File (Ctrl + N)");
    QString s2("Created by Tab_Al");

    if(QString::compare(ui->statusBar->currentMessage(), s1) == 0)
    {
        ui->statusBar->showMessage(s2);
    }
    else if(QString::compare(ui->statusBar->currentMessage(), s2) == 0)
    {
        ui->statusBar->showMessage(s1);
    }
}
