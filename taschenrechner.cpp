#include "Taschenrechner.h"
#include "ui_taschenrechner.h"

using namespace std;

istream* input;            //istream-pointer auf Input


enum Token_value {     // Das sind die Liste der Werten für die jeweilgen Rechenoperation die der Token erkönnen kann
    NAME,		NUMBER,		END,
    PLUS='+',	MINUS='-',	MUL='*',	DIV='/',
    PRINT=';',	ASSIGN='=',	LP='(',		RP=')'
};
int no_of_errors;
Token_value curr_tok = PRINT;                       // Token-Value=print(Print ist standard wert für token)
double number_value;
string string_value;
double error(const char* s)
{
    no_of_errors++;
    cerr << "error: " << s << '\n';
    return 1;
}

Token_value get_token()
{
    char ch;

    do {                                                // skip whitespace except '\en'; wenn nichts mehr im Stream ist token=end
        if(!input->get(ch)) return curr_tok = END;
    } while (ch!='\n' && isspace(ch));

    switch (ch) {
    case ';':                                           //falls char ; oder \n token=print
    case '\n':
        return curr_tok=PRINT;
    case '*':                                           //falls char rechenoperator oder klammern oder = dann entspricht der token dem korrespondierendem Token aus enum
    case '/':
    case '+':
    case '-':
    case '(':
    case ')':
    case '=':
        return curr_tok=Token_value(ch);
    case '0': case '1': case '2': case '3': case '4':   // falls char Ziffer oder Punkt dann wird char in den input gepackt
    case '5': case '6': case '7': case '8': case '9':   // danach number_value ist gleich der char der in den Stream gepackt wurde
    case '.':
        input->putback(ch);
        *input >> number_value;
        return curr_tok=NUMBER;                         // token ist gleich number
    default:			// NAME, NAME=, or error
        if (isalpha(ch)) {                              // falls char ein Buchstabe ist
            string_value = ch;
            while (input->get(ch) && isalnum(ch))       // Solange input da ist und ch ist alphanumerisch
                string_value += ch;                     // string_value.push_back(ch);
            // to work around library bug
            input->putback(ch);                         // char wird in den Stream gepackt
            return curr_tok=NAME;                       // token gleich name
        }
        error("bad token");
        return curr_tok=PRINT;
    }
}

map<string,double> table;                               // schafft relation zwischen zwei Werten; unidirektionaler Zugriff key->value

double expr(bool);                                      // Funktions dekleration

double prim(bool get)		// handle primaries
{
    if (get) get_token();                               // solange get true ist get_token ausführen

    switch (curr_tok) {
    case NUMBER:		// floating-point constant
    {	double v = number_value;                        // falls token Number
        get_token();
        return v;
    }
    case NAME:                                          // falls token name
    {	double& v = table[string_value];                // zeigt auf speicher
        if (get_token() == ASSIGN) v = expr(true);      // falls token == ASSIGN v=expr
        return v;
    }
    case MINUS:		// unary minus
        return -prim(true);                             //falls token==- wird -prim(true) ausgegeben
    case LP:                                            //falls token==(
    {	double e = expr(true);
        if (curr_tok != RP) return error(") expected");
        get_token();		// eat ')'
        return e;
    }
    default:
        return error("primary expected");
    }
}

double term(bool get)		// multiply and divide
{
    double left = prim(get);            //left ist gleich ergebnis von prim(get)

    while(true)
        switch (curr_tok) {
        case MUL:                       //falls token==mul
            left *= prim(true);         //left=left*returnwert von prim()
            break;
        case DIV:
            if (double d = prim(true)) {
                left /= d;
                break;
            }
            return error("divide by 0");
        default:
            return left;
        }
}

double expr(bool get)		// add and subtract
{
    double left = term(get);        //left ist gleich ergebnis von term()

    while(true)
        switch (curr_tok) {
        case PLUS:
            left += term(true);     //falls token plus left=left+returnwert von term()
            break;
        case MINUS:
            left -= term(true);
            break;
        default:
            return left;
        }
}



Taschenrechner::Taschenrechner(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Taschenrechner)
{
    ui->setupUi(this);
    keyNumberPressed();
    keyMathPressed();
}

Taschenrechner::~Taschenrechner()
{
    delete ui;
}

void Taschenrechner::keyNumberPressed()
{
    QPushButton *buttonsHolder[10];
    for (int i = 0; i < 10; i++) {
        QString button = "button" + QString::number(i);
        buttonsHolder[i] = Taschenrechner::findChild<QPushButton *>(button);
        connect(buttonsHolder[i], SIGNAL(clicked()), this, SLOT(number_pressed()));
    }
}

void Taschenrechner::keyMathPressed()
{
    connect(ui->ergebnis, SIGNAL(clicked()), this, SLOT(on_ergebnis_clicked()));
    connect(ui->additionButton, SIGNAL(clicked()), this, SLOT(math_button()));
    connect(ui->subtrahierenButton, SIGNAL(clicked()), this, SLOT(math_button()));
    connect(ui->multiplikationButton, SIGNAL(clicked()), this, SLOT(math_button()));
    connect(ui->dividereButton, SIGNAL(clicked()), this, SLOT(math_button()));
    connect(ui->klammerAuf, SIGNAL(clicked()), this, SLOT(math_button()));
    connect(ui->klammerZu, SIGNAL(clicked()), this, SLOT(math_button()));
    connect(ui->deleteAll, SIGNAL(clicked()), this, SLOT(math_button()));
}
void Taschenrechner::number_pressed()
{
    QPushButton *button = (QPushButton *)sender();
    QString butValue = button->text();
    QString displayValue = ui->display->text();
    ui->display->setText(displayValue + butValue);
}

//Hier wird
void Taschenrechner::on_ergebnis_clicked()
{
    QString task = ui->display->text();
    string taskstring=task.toStdString();       //strin=QString aus ui
    QString solution;
    stringstream b;     //Daten einlesen und schreiben hinterher
    b << taskstring;                        //b wird mit taskstring befüllt
    input = &b;                             //input-Stream zeigt auf Adresse vom stringstream
    double a;
    while (*input) {                        //während es den Input gibt
        get_token(); //holt immer den Token für jeden einzelnen Char)
        if (curr_tok == END) break;
        if (curr_tok == PRINT) continue;
        a = expr(false);
    }
    solution=QString::number(a,'g',10);
    ui->ausgabe->setText(solution);
}

void Taschenrechner::math_button()
{
    QPushButton *button = (QPushButton *)sender(); //passed a pointer of what has been clicked
    if (button->text() == "+") {
        ui->display->setText(ui->display->text() + "+");
    } else if (button->text() == "-") {
        ui->display->setText(ui->display->text() + "-");
    } else if (button->text() == "*") {
        ui->display->setText(ui->display->text() + "*");
    }
    else if (button->text() == "/") {
        ui->display->setText(ui->display->text() + "/");
    } else if (button->text() == "(") {
        ui->display->setText(ui->display->text() + "(");
    }
    else if (button->text() == ")") {
        ui->display->setText(ui->display->text() + ")");
    } else if (button->text() == "C") {
        ui->display->setText("");
        ui->ausgabe->setText("0");
    }
}
