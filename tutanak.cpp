#include "tutanak.h"
#include "ui_tutanak.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCheckBox>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QDir>
#include <QString>
#include <QList>
#include <QWidget>
#include <QKeyEvent>

Tutanak::Tutanak(QWidget *parent) : // Constructor güncellendi
    QDialog(parent),
    ui(new Ui::Tutanak)
{
    ui->setupUi(this);
    qDebug() << "Tutanak constructor çağrıldı";

    ui->lineEdit_10->installEventFilter(this);
    ui->lineEdit_11->installEventFilter(this);

    setTabOrder(ui->lineEdit_10, ui->lineEdit_11);
    setTabOrder(ui->lineEdit_11, ui->pushButtonKayit);

    // Malzeme ekle butonu ile malzeme listesini yükleyen fonksiyon bağlanacak
    connect(ui->pushButtonMek, &QPushButton::clicked, this, &Tutanak::on_pushButtonMek_clicked);
    // Kaydet butonu ile seçilen verilerin dosyaya yazılacağı fonksiyon
    connect(ui->pushButtonKayit, &QPushButton::clicked, this, &Tutanak::on_pushButtonKayit_clicked);
    // Yazdır butonu ile PDF veya TXT çıktısı alınacak fonksiyon
    connect(ui->pushButtonYaz, &QPushButton::clicked, this, &Tutanak::on_pushButtonYaz_clicked);
}

Tutanak::~Tutanak()
{
    delete ui;
}

// Proje yolunu ayarla
void Tutanak::setProjeYolu(const QString &yol)
{
    projeYolu = yol;
    //cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    qDebug() << "Tutanak::setProjeYolu çağrıldı, projeYolu: " << projeYolu;
}

// Proje adını ayarla
void Tutanak::setProjeAdi(const QString &adi)
{
    if (adi.isEmpty()) return;
    projeAdi = adi;
    //cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    qDebug() << "Tutanak::setProjeAdi çağrıldı, projeAdi: " << projeAdi;
}

// Event Filter
bool Tutanak::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            QWidget::focusNextChild();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

// Malzeme Ekle
void Tutanak::on_pushButtonMek_clicked()
{
    qDebug() << "on_pushButtonMek_clicked() çalıştı";

    if (projeYolu.isEmpty() || projeAdi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje bilgileri eksik!");
        return;
    }

    cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");

    if (cikisDosyaAdi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje bilgileri eksik!");
        return;
    }

    qDebug() << "cikisDosyaAdi: " << cikisDosyaAdi;
    if (!QFile::exists(cikisDosyaAdi)) {
        QMessageBox::warning(this, "Hata", "Çıkış dosyası bulunamadı: " + cikisDosyaAdi);
        return;
    }

    QFile file(cikisDosyaAdi);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Hata", "Çıkış dosyası açılamadı:\n" + cikisDosyaAdi + "\n" + file.errorString());
        return;
    }

    QTextStream in(&file);
    ui->tableWidget_tutanak->setRowCount(0);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        qDebug() << "Satır: " << line;
        qDebug() << "Alan sayısı: " << fields.size();
        for (int i = 0; i < fields.size(); ++i) {
            qDebug() << "Alan " << i << ": " << fields[i];
        }

        if (fields.size() >= 2) {
            int row = ui->tableWidget_tutanak->rowCount();
            ui->tableWidget_tutanak->insertRow(row);

            QTableWidgetItem *itemMalzeme = new QTableWidgetItem(fields[0].trimmed());
            QTableWidgetItem *itemMiktar = new QTableWidgetItem(fields[1].trimmed());
            QCheckBox *checkBox = new QCheckBox();

            ui->tableWidget_tutanak->setItem(row, 0, itemMalzeme);
            ui->tableWidget_tutanak->setItem(row, 1, itemMiktar);
            ui->tableWidget_tutanak->setCellWidget(row, 2, checkBox);
        }
    }

    file.close();

    qDebug() << "on_pushButtonMek_clicked() bitti";
}

// Kaydet
void Tutanak::on_pushButtonKayit_clicked()
{
    if (projeYolu.isEmpty() || projeAdi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje yolu veya adı eksik!");
        return;
    }

    QString filename = QDir(projeYolu).filePath(ui->lineEdit_10->text().trimmed() + "_tutanak.txt");

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Hata", "Tutanak dosyası oluşturulamadı!");
        return;
    }

    QTextStream out(&file);
    out << "Teslim Alan: " << ui->lineEdit_10->text() << "\n";
    out << "Tarih: " << ui->lineEdit_11->text() << "\n";
    out << "Teslim Edilen Malzemeler:\n";
    for (int row = 0; row < ui->tableWidget_tutanak->rowCount(); ++row) {
        QWidget *widget = ui->tableWidget_tutanak->cellWidget(row, 2);
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget);
        if (checkBox && checkBox->isChecked()) {
            QString malzeme = ui->tableWidget_tutanak->item(row, 0)->text();
            QString miktar = ui->tableWidget_tutanak->item(row, 1)->text();
            out << "- " << malzeme << " (" << miktar << ")\n";
        }
    }
    ui->lineEdit_10->clear();
    ui->lineEdit_11->clear();
    ui->lineEdit_10->setFocus();

    file.close();
    QMessageBox::information(this, "Kayıt", "Tutanak başarıyla kaydedildi:\n" + filename);
}

// Yazdır
void Tutanak::on_pushButtonYaz_clicked()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QPainter painter(&printer);
    int yPos = 100;
    painter.drawText(100, yPos, "Teslim Alan: " + ui->lineEdit_10->text());
    yPos += 30;
    painter.drawText(100, yPos, "Tarih: " + ui->lineEdit_11->text());
    yPos += 30;
    painter.drawText(100, yPos, "Teslim Edilen Malzemeler:");
    yPos += 30;

    for (int row = 0; row < ui->tableWidget_tutanak->rowCount(); ++row) {
        QWidget *widget = ui->tableWidget_tutanak->cellWidget(row, 2);
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget);
        if (checkBox && checkBox->isChecked()) {
            QString malzeme = ui->tableWidget_tutanak->item(row, 0)->text();
            QString miktar = ui->tableWidget_tutanak->item(row, 1)->text();
            painter.drawText(100, yPos, "- " + malzeme + " (" + miktar + ")");
            yPos += 30;
        }
    }

    painter.end();
    QMessageBox::information(this, "Yazdır", "Tutanak başarıyla yazdırıldı.");
}
