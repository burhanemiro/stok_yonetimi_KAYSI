#ifndef MALZEMECIKISFORMU_H
#define MALZEMECIKISFORMU_H

#include <QWidget>
#include <QMessageBox>
#include <QKeyEvent>
#include"ui_malzemecikisformu.h"
#include <QDialog>
#include <QMainWindow>
#include <QListWidget>
#include "tutanak.h"
#include <QString>
#include <QListWidgetItem>
#include <QMap>
#include "malzemestokformu.h"
#include "tutanak.h"

namespace Ui {
class MalzemeCikisFormu;
}

class MalzemeCikisFormu : public QDialog
{
    Q_OBJECT

public:
    explicit MalzemeCikisFormu(QWidget *parent = nullptr, const QString &yol = "", const QString &adi = "");
    ~MalzemeCikisFormu();
    void setProjeAdi(const QString &adi);
    void setProjeYolu(const QString &yol); // Proje yolunu ayarlamak için fonksiyon
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MalzemeCikisFormu *ui;

    // Veri yapıları
    QMap<QString, int> stokMap; // Stok verilerini tutar (Ürün Adı -> Stok Miktarı)
    QStringList urunAdlariListesi; // Ürün adlarını tutan liste
    QListWidget *onerilerListesi;  // Öneriler listesi
    QString projeYolu;
    QString projeAdi;
    QString cikisDosyaAdi;
    QString stokDosyaAdi;
    // Yardımcı metodlar
    void urunAdlariYukle();       // Ürün adlarını öneri listesi için yükler
    bool stokYeterliMi(const QString &urunAdi, int miktar, int &mevcutStok);
    MalzemeStokFormu *malzemeStokFormu;  // Stok formu penceresi
    Tutanak *tutanak;        // Tutanak formu penceresi
    void guncelleDosyaYollari();  // Dosya yollarını güncelle
private slots:
    void on_pushButtonKaydet2_clicked();      // Kaydet butonu işlemleri
    void on_pushButtonStokDurumu2_clicked(); // Stok durumu buton işlemleri
    void on_pushButtonTutanakli_clicked();   // Tutanaklı buton işlemleri
    void on_lineEdit_5_textChanged(const QString &text); // Öneri listesi için arama kutusu
    void onerilerdenSecildi(QListWidgetItem *item);      // Öneri listesinden seçim işlemi
    void stokVerileriniYukle();
};

#endif // MALZEME_CIKIS_FORMU_H
