#ifndef INSIDETAB_H
#define INSIDETAB_H

#include <QWidget>

namespace Ui {
class InsideTab;
}

class InsideTab : public QWidget
{
    Q_OBJECT

public:
    explicit InsideTab(QWidget *parent, QString filename, QString content, bool ifNew = false);
    ~InsideTab();

    bool getIsNew() const;

    void setIsNew(bool value);

private:
    Ui::InsideTab *ui;
    QString filename = "";
    bool isNew = false;
};

#endif // INSIDETAB_H
