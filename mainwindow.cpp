#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_6->hide();

    ui->tableWidget->setColumnWidth(0, 50);
    ui->tableWidget->setColumnWidth(1, 70);
    ui->tableWidget->setColumnWidth(2, 120);
    ui->tableWidget->setColumnWidth(3, 60);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);

    ui->tableWidget_2->setColumnWidth(0, 50);
    ui->tableWidget_2->setColumnWidth(1, 70);
    ui->tableWidget_2->setColumnWidth(2, 150);
    ui->tableWidget_2->setColumnWidth(3, 60);

    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);

    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// microprogram button
void MainWindow::on_pushButton_clicked()
{
    QVector<QString> F1_instructoin = {"ADD","CLRAC","INCAC","DRTAC","DRTAR","PCTAR","WRITE"};
    QVector<QString> F2_instructoin = {"SUB","OR","AND","READ","ACTDR","INCDR","PCTDR"};
    QVector<QString> F3_instructoin = {"XOR","COM","SHL","SHR","INCPC","ARTPC"};
    QVector<QString> conflict1 = {"CLRAC","INCAC","SUB","OR","AND","ADD","XOR","COM","SHR","SHL"};
    QVector<QString> conflict2 = {"READ", "WRITE"};

    if(addedMicroprogram)
    {
        QMessageBox::warning(nullptr, "warning", "Microprogram was added previously." );
        return;
    }

    clear_microprogram_table();

    QStringList code = ui->textEdit->toPlainText().split("\n");

    for (int i = 0; i < code.size(); i++) {
        code[i] = code[i].trimmed();
    }

    code.removeAll(QString(""));

    if(code.length() == 0)
    {
        return;
    }

    int ponter;
    QVector<QPair<int, QString>> symolTable;

    for (int i = 0; i < code.length(); ++i) {
        QStringList line = code[i].trimmed().split(" ");
        ponter++;

        if(line[0] == "ORG")
        {
            ponter = line[1].toInt();
            continue;
        }

        if(line[0] != "ORG" && i == 0)
        {
            QMessageBox::warning(nullptr, "warning", "Microprogram must start with ORG." );
            return;
        }

        if(line[0] != "END" && i == (code.length() - 1))
        {
            QMessageBox::warning(nullptr, "warning", "Microprogram must finished with END." );
            return;
        }


        if(line[0].contains(":"))
        {
            QPair<int, QString> myPair(ponter-1, line[0].left(line[0].length() - 1));
            symolTable.push_back(myPair);
        }

        for (int i = 0; i < 128; ++i) {
            ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(QString::number(i)));
        }
    }

    code.pop_back();


    for (int i = 0; i < code.length(); ++i) {

        QStringList line = code[i].trimmed().split(" ");
        ponter++;
        QString F1 = "000";
        QString F2 = "000";
        QString F3 = "000";
        QString label = "";
        QString condition;
        QString branch;
        QString address = "0000000";

        if(line[0] == "ORG")
        {
            ponter = line[1].toInt();
            continue;
        }


        if(line[0].contains(":"))
        {
            label = line[0].left(line[0].length() - 1);
            line.pop_front();
        }


        QString instruction = line.join(" ");

        QStringList instruc = line[0].split(",");

        int counter = 0;
        for (int i = 0; i < instruc.length(); ++i) {
            if(conflict1.contains(instruc[i]))
                counter++;

        }

        if (counter > 1) {
            QMessageBox::information(nullptr, "Information", "Error in line : " + QString::number(ponter));
            clear_microprogram_table();
            return;
        }


        counter = 0;
        for (int i = 0; i < instruc.length(); ++i) {
            if(conflict2.contains(instruc[i]))
                counter++;

        }

        if (counter > 1) {
            QMessageBox::information(nullptr, "Information", "Error in line : " + QString::number(ponter));
            clear_microprogram_table();
            return;
        }

        for (int i = 0; i < instruc.length(); i++) {
            if (F1_instructoin.indexOf(instruc[i]) >= 0)
                F1 = padstart(QString::number((F1_instructoin.indexOf(instruc[i]) + 1),2), 3, '0');

            if (F2_instructoin.indexOf(instruc[i]) >= 0)
                F2 = padstart(QString::number((F2_instructoin.indexOf(instruc[i]) + 1),2), 3, '0');

            if (F3_instructoin.indexOf(instruc[i]) >= 0)
                F3 = padstart(QString::number((F3_instructoin.indexOf(instruc[i]) + 1),2), 3, '0');
        }

        condition = line[1];

        if(condition == "U")
            condition = "00";

        else if(condition == "I")
            condition = "01";

        else if(condition == "S")
            condition = "10";

        else if(condition == "Z")
            condition = "11";

        branch = line[2];

        if(branch == "JMP")
            branch = "00";

        else if(branch == "CALL")
            branch = "01";

        else if(branch == "RET")
            branch = "10";

        else if(branch == "MAP")
            branch = "11";

        if (branch == "00" || branch == "01")
        {
            address = line[3];

            if(address == "NEXT")
            {
                address = QString::number(ponter, 2);
            }

            else
            {
                bool flag = true;
                for (int i = 0; i < symolTable.length(); i++) {
                    if (symolTable[i].second == address) {
                        flag = false;
                        address = QString::number(symolTable[i].first, 2);
                    }
                }

                if (flag) {
                    clear_microprogram_table();
                    QMessageBox::warning(nullptr, "warning", address + " is not decleared" );
                    return;
                }
            }

        address = padstart(address, 7, '0');;

        }

        QString instruction_code = F1 + F2 + F3 + condition + branch + address;
        update(ponter,label,instruction,"0x" + binaryToHex(instruction_code).toUpper());
    }

    for (int row = 0; row < ui->tableWidget_2->rowCount(); row++) {
        for (int col = 0; col < ui->tableWidget_2->columnCount(); col++) {
        QTableWidgetItem *item = ui->tableWidget_2->item(row, col);
        if (item != nullptr) {
                item->setTextAlignment(Qt::AlignCenter);
        }
        }
    }

    addedMicroprogram = true;
    QMessageBox::information(nullptr, "Information", "Microprogram added succesfully." );
}

void MainWindow::on_textEdit_textChanged()
{
    addedMicroprogram = false;
}

// step by step button
void MainWindow::on_pushButton_5_clicked()
{
    if(!addedProgram)
    {
        QMessageBox::warning(nullptr, "warning", "At first, you should assemble program" );
        return;
    }



    if (firstStep) {
        on_pushButton_2_clicked();
        reset();
        for(int i = 0; i < ui->tableWidget_2->columnCount(); i++) {
        QTableWidgetItem *item = ui->tableWidget_2->item(64, i);
        item->setBackground(QBrush(Qt::yellow));
        ui->tableWidget_2->scrollToItem(item, QAbstractItemView::EnsureVisible);
        ui->tableWidget_2->setCurrentItem(item);
        }
        firstStep = false;
        return;
    }

    executeOneLine();

    if(ui->lineEdit_29->text() == "11111111111")
    {
        firstStep = true;
        ui->pushButton_6->hide();

        for(int i = 0; i < ui->tableWidget_2->rowCount(); i++) {
            for(int j = 0; j < ui->tableWidget_2->columnCount(); j++) {
                    QTableWidgetItem *item = ui->tableWidget_2->item(i, j);
                    item->setBackground(QBrush(Qt::white));
            }
        }
        return;
    }

    int temp = twoComplementToDecimal("0" + ui->lineEdit_5->text());
    for(int i = 0; i < ui->tableWidget_2->columnCount(); i++) {
        QTableWidgetItem *item = ui->tableWidget_2->item(temp, i);
        item->setBackground(QBrush(Qt::yellow));
        ui->tableWidget_2->scrollToItem(item, QAbstractItemView::EnsureVisible);
        ui->tableWidget_2->setCurrentItem(item);
    }

    ui->pushButton_6->setVisible(true);
}

void MainWindow::update(int line, QString label, QString instruction, QString content)
{
    ui->tableWidget_2->setItem(line-1,1,new QTableWidgetItem(label));
    ui->tableWidget_2->setItem(line-1,2,new QTableWidgetItem(instruction));
    ui->tableWidget_2->setItem(line-1,3,new QTableWidgetItem(content));
}

void MainWindow::update_memory(int line, QString label, QString instruction, QString content)
{
    ui->tableWidget->setItem(line-1,1,new QTableWidgetItem(label));
    ui->tableWidget->setItem(line-1,2,new QTableWidgetItem(instruction));
    ui->tableWidget->setItem(line-1,3,new QTableWidgetItem(content));
}

QString MainWindow::onesComplement(QString binaryString)
{
    QString ones = "";
    for (int i = 0; i < binaryString.length(); i++) {
        if (binaryString[i] == '0') {
        ones.append("1");
        } else {
        ones.append("0");
        }
    }
    return ones;
}

QString MainWindow::OR(QString str1, QString str2)
{
    QString result = "";
    for (int i = 0; i < str1.length(); i++) {
        if (str1[i] == '1' || str2[i] == '1') {
        result += "1";
        } else {
        result += "0";
        }
    }

    return result;
}

void MainWindow::reset()
{
    ui->lineEdit_25->setText("0");
    ui->lineEdit_26->setText("0x0000");
    ui->lineEdit_27->setText("0x0000");
    ui->lineEdit_28->setText("00000000000");
    ui->lineEdit_29->setText("00000000000");
    ui->lineEdit_30->setText("0000000");
    ui->lineEdit_5->setText("1000000");
    ui->lineEdit_22->setText("0");
    ui->lineEdit_23->setText("0000");
    ui->lineEdit_24->setText("00000000000");
    ui->lineEdit->setText("110");
    ui->lineEdit_2->setText("000");
    ui->lineEdit_3->setText("000");
    ui->lineEdit_4->setText("00");
    ui->lineEdit_7->setText("00");
    ui->lineEdit_6->setText("1000001");
}

void MainWindow::clear_microprogram_table()
{
    for (int i = 0; i < 128; ++i) {
        for (int j = 0; j < 4; ++j) {
        ui->tableWidget_2->setItem(i,j,new QTableWidgetItem(""));
        }
    }
}

void MainWindow::clear_program_table()
{
    for (int i = 0; i < 128; ++i) {
        for (int j = 0; j < 4; ++j) {
        ui->tableWidget->setItem(i,j,new QTableWidgetItem(""));
        }
    }
}

int MainWindow::getContentByLine(QString label)
{
    for (int i = 0; i < 128; i+=4) {
        if(ui->tableWidget_2->item(i,1)->text() == label)
        {
            return i;
        }
    }

    return -1;
}

void MainWindow::executeOneLine()
{
    int tmp = twoComplementToDecimal("0" + ui->lineEdit_5->text());

    for(int i = 0; i < ui->tableWidget_2->columnCount(); i++) {
        QTableWidgetItem *item = ui->tableWidget_2->item(tmp, i);
        item->setBackground(QBrush(Qt::white));
    }

    QString AC = ui->lineEdit_26->text();
    QString AR = ui->lineEdit_29->text();
    QString DR = ui->lineEdit_27->text();
    QString PC = ui->lineEdit_28->text();

    QString F1 = ui->lineEdit->text();

    if(F1 == "001")
    {
        QString result = binaryAddition(hexToBin(AC), hexToBin(DR));
        ui->lineEdit_25->setText(result[0]);
        result = result.mid(1);
        ui->lineEdit_26->setText("0x" + binaryToHex(result));
    }

    else if(F1 == "010")
    {
        ui->lineEdit_26->setText("0x0000");
    }

    else if(F1 == "011")
    {
        if(AC == "0xFFFF")
        {
            ui->lineEdit_26->setText("0x0000");
        }

        else
        {
            int temp = twoComplementToDecimal(hexToBin((AC)));
            temp++;
            ui->lineEdit_26->setText("0x" + binaryToHex(toTwoComplement(QString::number(temp))));
        }
    }

    else if(F1 == "100")
    {
        ui->lineEdit_26->setText(DR);
    }

    else if(F1 == "101")
    {
        ui->lineEdit_29->setText(hexToBin(DR.mid(3, 3)).mid(1));
        if(ui->lineEdit_29->text() == "11111111111")
            return;
    }

    else if(F1 == "110")
    {
        ui->lineEdit_29->setText(PC);
    }

    else if(F1 == "111")
    {
        ui->tableWidget->setItem(twoComplementToDecimal("0" + AR), 3,new QTableWidgetItem(DR));
        for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
            for (int col = 0; col < ui->tableWidget->columnCount(); col++) {
                QTableWidgetItem *item = ui->tableWidget->item(row, col);
                if (item != nullptr) {
                    item->setTextAlignment(Qt::AlignCenter);
                }
            }
        }
    }


    QString F2 = ui->lineEdit_2->text();

    if(F2 == "001")
    {
        QString str = onesComplement(hexToBin(DR));
        str = binaryAddition(str, "0000000000000001").mid(1);
        QString ac = hexToBin(AC);
        QString result = binaryAddition(hexToBin(AC), str);
        ui->lineEdit_25->setText(result[0]);
        result = result.mid(1);
        ui->lineEdit_26->setText("0x" + binaryToHex(result));
    }

    else if(F2 == "010")
    {
        ui->lineEdit_26->setText("0x" + binaryToHex(OR(hexToBin(AC), hexToBin(DR))));
    }

    else if(F2 == "011")
    {
        ui->lineEdit_26->setText("0x" + binaryToHex(AND(hexToBin(AC), hexToBin(DR))));
    }

    else if(F2 == "100")
    {
        ui->lineEdit_27->setText(ui->tableWidget->item(twoComplementToDecimal(AR), 3)->text());
    }

    else if(F2 == "101")
    {
        ui->lineEdit_27->setText(AC);
    }

    else if(F2 == "110")
    {
        if(DR == "0xFFFF")
        {
            ui->lineEdit_27->setText("0x0000");
        }

        else
        {
            int temp = twoComplementToDecimal(hexToBin((DR)));
            temp++;
            ui->lineEdit_27->setText("0x" + binaryToHex(toTwoComplement(QString::number(temp))));
        }
    }

    else if(F2 == "111")
    {
        ui->lineEdit_27->setText("0x" + binaryToHex(hexToBin(DR).mid(0, 5) + ui->lineEdit_28->text()));
    }


    QString F3 = ui->lineEdit_3->text();

    if(F3 == "001")
    {
        ui->lineEdit_26->setText("0x" + binaryToHex(XOR(hexToBin(AC), hexToBin(DR))));
    }

    else if(F3 == "010")
    {
        ui->lineEdit_26->setText("0x" + binaryToHex(onesComplement(hexToBin(AC))));
    }

    else if(F3 == "011")
    {
        SHL(hexToBin(ui->lineEdit_26->text()), ui->lineEdit_25->text());
    }

    else if(F3 == "100")
    {
        SHR(hexToBin(ui->lineEdit_26->text()), ui->lineEdit_25->text());
    }

    else if(F3 == "101")
    {
        if(PC == "11111111111")
        {
            ui->lineEdit_28->setText("00000000000");
        }

        else
        {
            int temp = twoComplementToDecimal("0" + PC);
            temp++;

            QStringList binaryList;
            while (temp > 0) {
                int remainder = temp % 2;
                binaryList.prepend(QString::number(remainder));
                temp /= 2;
            }

            ui->lineEdit_28->setText(padstart(binaryList.join(""), 11, '0'));
        }
    }

    else if(F3 == "110")
    {
        ui->lineEdit_28->setText(AR);
    }

    bool condition;
    QString cd = ui->lineEdit_4->text();

    if(cd == "00")
    {
        condition = true;
    }

    else if(cd == "01")
    {
        condition = (ui->lineEdit_22->text() == "1");
    }

    else if(cd == "10")
    {
        condition = (hexToBin(ui->lineEdit_26->text())[0] == '1');
    }

    else if(cd == "11")
    {
        condition = (ui->lineEdit_26->text() == "0x0000");
    }

    QString br = ui->lineEdit_7->text();
    QString ad = ui->lineEdit_6->text();

    if(br == "00")
    {
        if(condition)
        {
            ui->lineEdit_5->setText(ad);
        }

        else
        {
            int temp = twoComplementToDecimal("0" + ui->lineEdit_5->text());
            temp++;

            QStringList binaryList;
            while (temp > 0) {
                int remainder = temp % 2;
                binaryList.prepend(QString::number(remainder));
                temp /= 2;
            }

            ui->lineEdit_5->setText(padstart(binaryList.join(""), 7, '0'));
        }
    }

    else if(br == "01")
    {
        int temp = twoComplementToDecimal("0" + ui->lineEdit_5->text());
        temp++;

        QStringList binaryList;
        while (temp > 0) {
            int remainder = temp % 2;
            binaryList.prepend(QString::number(remainder));
            temp /= 2;
        }

        QString carplusone = padstart(binaryList.join(""), 7, '0');

        if(condition)
        {
            ui->lineEdit_30->setText(carplusone);
            ui->lineEdit_5->setText(ad);
        }

        else
        {
            ui->lineEdit_5->setText(carplusone);
        }
    }

    else if(br == "10")
    {
        ui->lineEdit_5->setText(ui->lineEdit_30->text());
    }

    else if(br == "11")
    {
        ui->lineEdit_22->setText(hexToBin(ui->lineEdit_27->text())[0]);
        ui->lineEdit_5->setText("0" + hexToBin(ui->lineEdit_27->text()).mid(1, 4) + "00");
        ui->lineEdit_24->setText(hexToBin(ui->lineEdit_27->text()).mid(5));
    }

    int temp = twoComplementToDecimal("0" + ui->lineEdit_5->text());

    QString next_line = hexToBin(ui->tableWidget_2->item(temp, 3)->text());
    ui->lineEdit->setText(next_line.mid(0, 3));
    ui->lineEdit_2->setText(next_line.mid(3, 3));
    ui->lineEdit_3->setText(next_line.mid(6, 3));
    ui->lineEdit_4->setText(next_line.mid(9, 2));
    ui->lineEdit_7->setText(next_line.mid(11, 2));
    ui->lineEdit_6->setText(next_line.mid(13));
}

void MainWindow::on_textEdit_2_textChanged()
{
    addedProgram = false;
}

QString MainWindow::binaryAddition(QString binStr1, QString binStr2)
{
    int lenDiff = abs(binStr1.length() - binStr2.length());
    if (binStr1.length() < binStr2.length()) {
        binStr1.prepend(QString(lenDiff, '0'));
    } else if (binStr2.length() < binStr1.length()) {
        binStr2.prepend(QString(lenDiff, '0'));
    }

    QString result;
    int carry = 0;
    for (int i = binStr1.length() - 1; i >= 0; i--) {
        int sum = binStr1[i].digitValue() + binStr2[i].digitValue() + carry;
        if (sum == 0) {
            result.prepend('0');
            carry = 0;
        } else if (sum == 1) {
            result.prepend('1');
            carry = 0;
        } else if (sum == 2) {
            result.prepend('0');
            carry = 1;
        } else {
            result.prepend('1');
            carry = 1;
        }
    }

    if (carry != 0) {
        result.prepend('1');
    }

    else
    {
        result.prepend('0');
    }

    return result;
}

QString MainWindow::AND(QString binStr1, QString binStr2)
{
    QString result;
    for (int i = 0; i < binStr1.length(); i++)
    {
        if (binStr1.mid(i, 1) == "1" && binStr2.mid(i, 1) == "1") {
            result.append("1");
        } else {
            result.append("0");
        }
    }

    return result;
}

QString MainWindow::XOR(QString binStr1, QString binStr2)
{
    QString result;
    for (int i = 0; i < binStr1.length(); i++)
    {
        if (binStr1.mid(i, 1) == binStr2.mid(i, 1)) {
            result.append("0");
        } else {
            result.append("1");
        }
    }

    return result;
}

void MainWindow::SHL(QString binStr, QString carry)
{
    binStr.append(ui->lineEdit_25->text());
    ui->lineEdit_25->setText(binStr[0]);
    binStr = binStr.mid(1);
    ui->lineEdit_26->setText("0x" + binaryToHex(binStr));
}

void MainWindow::SHR(QString binStr, QString carry)
{
    binStr.prepend(ui->lineEdit_25->text());
    ui->lineEdit_25->setText(binStr[binStr.length()-1]);
    binStr.remove(binStr.length() - 1, 1);
    ui->lineEdit_26->setText("0x" + binaryToHex(binStr));
}

void MainWindow::on_pushButton_2_clicked()
{
    if(!firstStep && addedProgram)
    {
        QMessageBox::warning(nullptr, "warning", "Program was added previously." );
        return;
    }

    if(!addedMicroprogram)
    {
        QMessageBox::warning(nullptr, "warning", "At first, MicroProgram should add." );
        return;
    }

    clear_program_table();

    QStringList code = ui->textEdit_2->toPlainText().split("\n");

    for (int i = 0; i < code.size(); i++) {
        code[i] = code[i].trimmed();
    }

    code.removeAll(QString(""));

    if(code.length() == 0)
    {
        return;
    }

    int ponter;
    QVector<QPair<int, QString>> symolTable;

    for (int i = 0; i < code.length(); ++i) {
        QStringList line = code[i].trimmed().split(" ");
        ponter++;

        if(line[0] == "ORG")
        {
            ponter = line[1].toInt();
            continue;
        }

        if(line[0] != "ORG" && i == 0)
        {
            QMessageBox::warning(nullptr, "warning", "Program must start with ORG." );
            return;
        }

        if(line[0] != "END" && i == (code.length() - 1))
        {
            QMessageBox::warning(nullptr, "warning", "Program must finished with END." );
            return;
        }


        if(line[0].contains(","))
        {
            QPair<int, QString> myPair(ponter-1, line[0].left(line[0].length() - 1));
            symolTable.push_back(myPair);
        }

        for (int i = 0; i < 2048; ++i) {
            ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(i)));
        }
    }

    code.pop_back();

    for (int i = 0; i < code.length(); ++i) {

        QStringList line = code[i].trimmed().split(" ");
        ponter++;
        QString label = "";
        QString I = "0";
        QString opcode;
        QString address = "00000000000";

        if(line[0] == "ORG")
        {
            ponter = line[1].toInt();
            continue;
        }


        if(line[0].contains(","))
        {
            label = line[0].left(line[0].length() - 1);
            line.pop_front();
        }

        QString instruction = line.join(" ");

        if (line[0] == "HEX") {
            QString instruction_code = "0x" + padstart(line[1], 4, '0');

            update_memory(ponter, label, instruction, instruction_code);
            continue;
        }

        if (line[0] == "DEC") {

            QString binary = toTwoComplement(line[1]);
            QString instruction_code = "0x" + padstart(binaryToHex(binary), 4, '0').toUpper();
            update_memory(ponter, label, instruction, instruction_code);
            continue;
        }

        if (line[0] == "HLT") {
            QString instruction_code = "0xFFFF";
            update_memory(ponter, label, instruction, instruction_code);
            continue;
        }

        if(line.length() == 3)
        {
            I = "1";
            line.pop_back();
        }

        if(line.length() == 2)
        {
            bool flag = true;
            for (int i = 0; i < symolTable.length(); i++) {
                if (symolTable[i].second == line[1]) {
                    flag = false;
                    address = QString::number(symolTable[i].first, 2);
                    break;
                }
            }

            if (flag) {
                clear_program_table();
                QMessageBox::warning(nullptr, "warning", line[1] + " is not decleared" );
                return;
            }

            address = padstart(address, 11, '0');
            line.pop_back();
        }

        int value = getContentByLine(line[0]);

        if(value == -1)
        {
            clear_program_table();
            QMessageBox::warning(nullptr, "warning", line[0] + " is not an instruction" );
            return;
        }

        else
        {
            opcode = padstart(QString::number(value/4, 2), 4, '0');
        }

        QString instruction_code = I + opcode + address;
        update_memory(ponter,label,instruction,"0x" + binaryToHex(instruction_code).toUpper());
    }

    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        for (int col = 0; col < ui->tableWidget->columnCount(); col++) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if (item != nullptr) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

    addedProgram = true;

    if(!firstStep)
        QMessageBox::information(nullptr, "Information", "Microprogram added succesfully." );

    firstStep = true;
}

void MainWindow::on_pushButton_3_clicked()
{
    QMessageBox::information(nullptr, "Information", "This project created by AmirHossein Zendevani" );
}

void MainWindow::on_pushButton_4_clicked()
{
    if(!addedProgram)
    {
        QMessageBox::warning(nullptr, "warning", "At first, you should assemble program" );
        return;
    }

    while(ui->lineEdit_29->text() != "11111111111")
    {
        executeOneLine();
    }
}

QString MainWindow::hexToBin(QString hexString)
{
    if(hexString.contains("0x"))
    {
        hexString.remove(0, 2);
    }

    QString binString;
    for (const QChar& c : hexString) {
        switch (c.toLatin1()) {
        case '0': binString += "0000"; break;
        case '1': binString += "0001"; break;
        case '2': binString += "0010"; break;
        case '3': binString += "0011"; break;
        case '4': binString += "0100"; break;
        case '5': binString += "0101"; break;
        case '6': binString += "0110"; break;
        case '7': binString += "0111"; break;
        case '8': binString += "1000"; break;
        case '9': binString += "1001"; break;
        case 'a': binString += "1010"; break;
        case 'b': binString += "1011"; break;
        case 'c': binString += "1100"; break;
        case 'd': binString += "1101"; break;
        case 'e': binString += "1110"; break;
        case 'f': binString += "1111"; break;
        case 'A': binString += "1010"; break;
        case 'B': binString += "1011"; break;
        case 'C': binString += "1100"; break;
        case 'D': binString += "1101"; break;
        case 'E': binString += "1110"; break;
        case 'F': binString += "1111"; break;
        }
    }
    return binString;
}

QString MainWindow::toTwoComplement(QString input)
{
    int value = input.toInt();
    bool isNeg = (value < 0);
    char temp= '0';

    if(isNeg)
    {
        temp = '1';
        value *= -1;
    }

    QString binaryString;
    do {
        binaryString.prepend(QString::number(value % 2));
        value /= 2;
    } while (value != 0);

    if(isNeg)
    {
        bool flag = true;

        for(int i = (binaryString.length() - 1); i >= 0; i--) {

            if(flag && binaryString[i] == '1')
            {
                flag = false;
                continue;
            }

            if(flag)
            {
                continue;
            }

            binaryString[i] = (binaryString[i] == '0') ? '1' : '0';
        }

    }

    binaryString = padstart(binaryString, 16 , temp);
    return binaryString;
}

int MainWindow::twoComplementToDecimal(QString binaryString)
{
    bool isNeg = (binaryString[0] == '1');

    if(isNeg)
    {
        bool flag = true;

        for(int i = (binaryString.length() - 1); i >= 0; i--) {

            if(flag && binaryString[i] == '1')
            {
                flag = false;
                continue;
            }

            if(flag)
            {
                continue;
            }

            binaryString[i] = (binaryString[i] == '0') ? '1' : '0';
        }

    }

    int result = binaryString.toInt(nullptr, 2);

    if(isNeg) return result * -1;
    return result;
}

void MainWindow::on_pushButton_6_clicked()
{
    firstStep = true;
    ui->pushButton_6->hide();
    for(int i = 0; i < ui->tableWidget_2->rowCount(); i++) {
        for(int j = 0; j < ui->tableWidget_2->columnCount(); j++) {
            QTableWidgetItem *item = ui->tableWidget_2->item(i, j);
            item->setBackground(QBrush(Qt::white));
        }
    }
}

