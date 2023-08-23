#ifndef TASCHENRECHNER_H
#define TASCHENRECHNER_H

#include <QMainWindow>
#include <iostream>
#include <string>
#include <sstream>
#include<strstream>
#include <cctype>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class Taschenrechner; }
QT_END_NAMESPACE

class Taschenrechner : public QMainWindow
{
    Q_OBJECT

public:
    Taschenrechner(QWidget *parent = nullptr);
    ~Taschenrechner();
    void keyNumberPressed();
    void keyMathPressed();
private:
    Ui::Taschenrechner *ui;
private slots:
    void number_pressed();
    void on_ergebnis_clicked();
    void math_button();
};
#endif // TASCHENRECHNER_H
