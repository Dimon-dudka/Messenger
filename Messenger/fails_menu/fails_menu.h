//  This widget describes widget that shows when
//  some network problems are

#pragma once

#include <QFont>
#include <QLabel>
#include <QWidget>
#include <QPointer>
#include <QVBoxLayout>
#include <QPushButton>

class fails_menu : public QWidget
{
    Q_OBJECT

private:

    QPointer<QVBoxLayout> widget_layout;
    QPointer<QPushButton> try_again_button;
    QPointer<QLabel> fail_label;

private slots:

    void try_again_button_slot();

public:
    fails_menu(QWidget *parrent = 0);

public slots:

    void open_fails_menu_slot(const QString &problem_type);

signals:
    void try_again_signal();
};
