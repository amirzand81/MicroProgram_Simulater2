#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_textEdit_textChanged();

    void on_pushButton_5_clicked();

    void on_pushButton_2_clicked();

    void on_textEdit_2_textChanged();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    QString hexToBin(QString hexString);
    bool firstStep{false};
    QString toTwoComplement(QString input);
    int twoComplementToDecimal(QString str);
    bool addedMicroprogram{false};
    bool addedProgram{false};
    void update(int line, QString label, QString instruction, QString content);
    void update_memory(int line, QString label, QString instruction, QString content);
    QString onesComplement(QString binaryString);
    QString OR(QString str1, QString str2);
    void reset();
    QString padstart(QString str, int target_len, QChar c) {
        if (str.length() < target_len) {
            int num_chars_to_add = target_len - str.length();
            QString prefix(num_chars_to_add, c);
            str = prefix + str;
        }
        return str;
    }
    QString binaryToHex(QString binary) {

        while (binary.length() % 4 != 0) {
            binary.prepend("0");
        }

        QString hex;
        for (int i = 0; i < binary.length(); i += 4) {
            QString nibble = binary.mid(i, 4);
            bool ok;
            int decimalValue = nibble.toInt(&ok, 2); // Convert the nibble to decimal
            if (!ok) {
                return ""; // Invalid binary string
            }
            hex += QString::number(decimalValue, 16); // Convert the decimal value to hexadecimal
        }

        return hex.toUpper();
    }
    void clear_microprogram_table();
    void clear_program_table();
    int getContentByLine(QString label);
    void executeOneLine();
    QString binaryAddition(QString binStr1, QString binStr2);
    QString AND(QString binStr1, QString binStr2);
    QString XOR(QString binStr1, QString binStr2);
    void SHL(QString binStr, QString carry);
    void SHR(QString binStr, QString carry);
};
#endif // MAINWINDOW_H
