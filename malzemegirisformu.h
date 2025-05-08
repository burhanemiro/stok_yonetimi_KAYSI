#ifndef MALZEMEGIRISFORMU_H
#define MALZEMEGIRISFORMU_H

#include <QWidget>
#include <QMessageBox>
#include <QKeyEvent>
#include "ui_malzemegirisformu.h"
#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
//#include <QString> // QString'i kaldırdık

namespace Ui {
class MalzemeGirisFormu;
}

class MalzemeGirisFormu : public QDialog
{
    Q_OBJECT

public:
    explicit MalzemeGirisFormu(QWidget *parent = nullptr);
    ~MalzemeGirisFormu();

    // Dinamik proje adı ayarlamak için
    void setProjeYolu(const QString &yol); // Proje yolunu ayarlamak için fonksiyon
    void setProjeAdi(const QString &adi); // Proje adını ayarlamak için fonksiyon

protected:
    // Event filter
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    // lineEdit_1 için metin değişikliği işlemi
    void on_lineEdit_1_textChanged(const QString &text);

    // Kaydet butonuna tıklandığında çalıştırılacak işlem
    void on_pushButtonKaydet_clicked();

    // Stok durumu butonuna tıklandığında çalıştırılacak işlem
    void on_pushButtonStokDurumu_clicked();

    // Önerilerden bir ürün seçildiğinde çalıştırılacak işlem
    void onerilerdenSecildi(QListWidgetItem *item);

private:
    Ui::MalzemeGirisFormu *ui;        // Kullanıcı arayüzü
    QString projeAdi; // Proje adı // QString'leri kaldırdık
    QString projeYolu;            // Dinamik proje adı
    QListWidget *onerilerListesi;     // Öneri listesi widget'ı
    QStringList urunAdlariListesi;
    // Ürün adlarını tutan liste
    QString cikisDosyaAdi;
    QString stokDosyaAdi;
    QString girisDosyaAdi;
    // Yardımcı işlevler
    void urunAdlariYukle();           // Ürün adlarını dosyadan yükler
};

#endif // MALZEMEGIRISFORMU_H
