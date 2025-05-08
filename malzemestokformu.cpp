#include "malzemeStokFormu.h"
#include "ui_malzemeStokFormu.h"
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QList>
#include <QWidget>

MalzemeStokFormu::MalzemeStokFormu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MalzemeStokFormu)
{
    ui->setupUi(this);
    connect(ui->pushButtonGoster, &QPushButton::clicked, this, &MalzemeStokFormu::on_pushButtonGoster_clicked);
    connect(ui->pushButtonYenile, &QPushButton::clicked, this, &MalzemeStokFormu::on_pushButtonYenile_clicked);
    connect(ui->lineEdit_8, &QLineEdit::textChanged, this, &MalzemeStokFormu::on_lineEdit_8_textChanged);

    //verileriYukle(); // Bu metot, setProjeAdi ve setProjeYolu'ndan sonra çağrılacak
}

MalzemeStokFormu::~MalzemeStokFormu()
{
    delete ui;
}

void MalzemeStokFormu::setProjeAdi(const QString &adi)
{
    if (adi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje adı boş olamaz!");
        return;
    }
    projeAdi = adi;
    //girisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_giris.txt");
    //cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    //stokDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_stok.txt");
    qDebug() << "setProjeAdi - projeAdi: " << projeAdi;
    verileriYukle();
}

void MalzemeStokFormu::setProjeYolu(const QString &yol)
{
    if (yol.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje yolu boş olamaz!");
        return;
    }
    projeYolu = yol;
    //girisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_giris.txt");
    //cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    //stokDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_stok.txt");
    qDebug() << "setProjeYolu - projeYolu: " << projeYolu;
    verileriYukle();
}

QMap<QString, int> MalzemeStokFormu::girisVerileriOku()
{
    QMap<QString, int> giris;
    if (projeYolu.isEmpty() || projeAdi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje adı veya yolu ayarlanmamış! Lütfen proje bilgilerini kontrol edin.");
        return giris;
    }
    girisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_giris.txt");
    QFile file(girisDosyaAdi);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString satir = in.readLine();
            QStringList parcalar = satir.split(",");
            if (parcalar.size() >= 2) {
                QString urunAdi = parcalar[0].trimmed();
                int miktar = parcalar[1].toInt();
                giris[urunAdi] += miktar;
            }
        }
        file.close();
    }
    return giris;
}

QMap<QString, int> MalzemeStokFormu::cikisVerileriOku()
{
    QMap<QString, int> cikis;
    if (projeYolu.isEmpty() || projeAdi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje adı veya yolu ayarlanmamış! Lütfen proje bilgilerini kontrol edin.");
        return cikis;
    }
    cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    QFile file(cikisDosyaAdi);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString satir = in.readLine();
            QStringList parcalar = satir.split(",");
            if (parcalar.size() >= 2) {
                QString urunAdi = parcalar[0].trimmed();
                int miktar = parcalar[1].toInt();
                cikis[urunAdi] += miktar;
            }
        }
        file.close();
    }
    return cikis;
}

void MalzemeStokFormu::verileriYukle()
{
    if (projeYolu.isEmpty() || projeAdi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje adı veya yolu ayarlanmamış! Lütfen proje bilgilerini kontrol edin.");
        return;
    }
    girisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_giris.txt");
    cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    stokDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_stok.txt");

    // TableWidget'ı temizle
    ui->tableWidget_stok->clearContents();
    ui->tableWidget_stok->setRowCount(0);
    // Kolon başlıklarını ayarla
    ui->tableWidget_stok->setColumnCount(4);
    QStringList basliklar = {"Ürün Adı", "Toplam Giriş", "Toplam Çıkış", "Stok"};
    ui->tableWidget_stok->setHorizontalHeaderLabels(basliklar);
    // Giriş ve çıkış verilerini oku
    QMap<QString, int> girisler = girisVerileriOku();
    QMap<QString, int> cikislar = cikisVerileriOku();
    // Tüm ürünlerin listesini oluştur
    QStringList urunler = girisler.keys();
    for (const QString &urun : cikislar.keys()) {
        if (!urunler.contains(urun)) {
            urunler.append(urun);
        }
    }

    // Eğer ürün listesi boşsa kullanıcıya bilgi ver
    if (urunler.isEmpty()) {
        QMessageBox::information(this, "Bilgi", "Stok bilgisi bulunamadı. Lütfen giriş ve çıkış dosyalarını kontrol edin.");
        return;
    }

    // Stok dosyasını aç
    QFile stokDosyasi(stokDosyaAdi);
    // stokDosyaAdi değişkeni kullanılacak
    if (!stokDosyasi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Hata", "Stok dosyası açılamadı: " + stokDosyasi.errorString());
        return;
    }

    QTextStream out(&stokDosyasi);
    out << "Ürün Adı,Toplam Giriş,Toplam Çıkış,Stok\n"; // Başlık satırı

    // Ürünleri tabloya ve stok dosyasına yaz
    int satir = 0;
    for (const QString &urun : urunler) {
        int giris = girisler.value(urun, 0);
        int cikis = cikislar.value(urun, 0);
        int stok = qMax(0, giris - cikis); // Stok negatifse sıfır yap

        // Debug: Eklenmeye çalışılan verileri göster
        qDebug() << "Eklenecek Ürün:" << urun << ", Giriş:" << giris << ", Çıkış:" << cikis << ", Stok:" << stok;
        // TableWidget'a ekleme
        ui->tableWidget_stok->insertRow(satir);
        ui->tableWidget_stok->setItem(satir, 0, new QTableWidgetItem(urun));
        ui->tableWidget_stok->setItem(satir, 1, new QTableWidgetItem(QString::number(giris)));
        ui->tableWidget_stok->setItem(satir, 2, new QTableWidgetItem(QString::number(cikis)));
        ui->tableWidget_stok->setItem(satir, 3, new QTableWidgetItem(QString::number(stok)));

        // Stok dosyasına yaz
        out << urun << "," << giris << "," << cikis << "," << stok << "\n";
        satir++;
    }

    stokDosyasi.close();

    qDebug() << "TableWidget Row Count:" << ui->tableWidget_stok->rowCount();
    qDebug() << "TableWidget Column Count:" << ui->tableWidget_stok->columnCount();
    QMessageBox::information(this, "Başarılı", "Stok bilgileri başarıyla yüklendi ve kaydedildi: " + stokDosyaAdi);
}

void MalzemeStokFormu::on_pushButtonGoster_clicked()
{
    verileriYukle();
}

void MalzemeStokFormu::on_pushButtonYenile_clicked()
{
    ui->tableWidget_stok->setRowCount(0);

    if (projeYolu.isEmpty() || projeAdi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje adı veya yolu ayarlanmamış! Lütfen proje bilgilerini kontrol edin.");
        return;
    }
    QFile file(QDir(projeYolu).filePath(projeAdi + "_stok.txt"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.close();
    }

    QMessageBox::information(this, "Bilgi", "Stok bilgileri temizlendi.");
}

void MalzemeStokFormu::on_lineEdit_8_textChanged(const QString &text)
{
    filtrele(text);
}

void MalzemeStokFormu::filtrele(const QString &aramaMetni)
{
    for (int i = 0; i < ui->tableWidget_stok->rowCount(); ++i) {
        bool eslesme = ui->tableWidget_stok->item(i, 0)->text().contains(aramaMetni, Qt::CaseInsensitive);
        ui->tableWidget_stok->setRowHidden(i, !eslesme);
    }
}
void MalzemeStokFormu::on_pushButtonYazdir_clicked()
{
    QString html;
    html += "<h2>Stok Durumu Raporu</h2>";
    html += "<table border='1' cellspacing='0' cellpadding='4'>";
    html += "<tr><th>Ürün Adı</th><th>Toplam Giriş</th><th>Toplam Çıkış</th><th>Stok</th></tr>";

    int rowCount = ui->tableWidget_stok->rowCount();
    int colCount = ui->tableWidget_stok->columnCount();
    for (int i = 0; i < rowCount; ++i) {
        html += "<tr>";
        for (int j = 0; j < colCount; ++j) {
            QString text = ui->tableWidget_stok->item(i, j) ?
                               ui->tableWidget_stok->item(i, j)->text() : "";
            html += "<td>" + text + "</td>";
        }
        html += "</tr>";
    }
    html += "</table>";

    QTextDocument doc;
    doc.setHtml(html);

    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        doc.print(&printer);
    }
}
