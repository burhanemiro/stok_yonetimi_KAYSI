#ifndef MALZEMESTOKFORMU_H
#define MALZEMESTOKFORMU_H

#include <QWidget>
#include <QMessageBox>
#include <QKeyEvent>
#include "ui_malzemeStokFormu.h"
#include <QDialog>
#include <QTextDocument>
#include <QString>
#include <QMap>
#include <QListWidgetItem>
#include <QList>

namespace Ui {
class MalzemeStokFormu;
}

class MalzemeStokFormu : public QDialog
{
    Q_OBJECT

public:
    explicit MalzemeStokFormu(QWidget *parent = nullptr);
    ~MalzemeStokFormu();

    // Proje adı ve yolu ayarlama
    void setProjeAdi(const QString &adi);
    void setProjeYolu(const QString &yol);

private slots:
    // Buton işlevleri
    void on_pushButtonGoster_clicked();
    void on_pushButtonYenile_clicked();
    void on_pushButtonYazdir_clicked();
    void on_lineEdit_8_textChanged(const QString &text);

private:
    Ui::MalzemeStokFormu *ui;

    // Dosya yolları
    QString projeAdi;
    QString projeYolu;
    QString girisDosyaAdi;
    QString cikisDosyaAdi;
    QString stokDosyaAdi;

    // Veri işleme
    QMap<QString, int> girisVerileriOku();
    QMap<QString, int> cikisVerileriOku();
    void verileriYukle();
    void filtrele(const QString &arama);
};

#endif // MALZEMESTOKFORMU_H
