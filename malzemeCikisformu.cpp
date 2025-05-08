#include "malzemecikisformu.h"
#include "ui_malzemecikisformu.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include "malzemeStokFormu.h"
#include <QListWidget>
#include "tutanak.h"
#include "ui_tutanak.h"
#include <QListWidgetItem>
#include <QFileDialog>

MalzemeCikisFormu::MalzemeCikisFormu(QWidget *parent) // Constructor güncellendi
    : QDialog(parent)
    , ui(new Ui::MalzemeCikisFormu)
{
    ui->setupUi(this);
    // Event Filter'lar
    ui->lineEdit_5->installEventFilter(this);
    ui->lineEdit_6->installEventFilter(this);
    ui->lineEdit_7->installEventFilter(this);

    // Tab sırası
    setTabOrder(ui->lineEdit_5, ui->lineEdit_6);
    setTabOrder(ui->lineEdit_6, ui->lineEdit_7);
    setTabOrder(ui->lineEdit_7, ui->pushButtonKaydet2);

    // Öneri listesi oluşturma
    onerilerListesi = new QListWidget(this);
    onerilerListesi->hide();
    // Buton bağlantıları
    connect(ui->pushButtonKaydet2, &QPushButton::clicked, this, &MalzemeCikisFormu::on_pushButtonKaydet2_clicked);
    connect(ui->pushButtonStokDurumu2, &QPushButton::clicked, this, &MalzemeCikisFormu::on_pushButtonStokDurumu2_clicked);
    connect(onerilerListesi, &QListWidget::itemClicked, this, &MalzemeCikisFormu::onerilerdenSecildi);
    connect(ui->lineEdit_5, &QLineEdit::textChanged, this, &MalzemeCikisFormu::on_lineEdit_5_textChanged);
    connect(ui->pushButtonTutanakli, &QPushButton::clicked, this, &MalzemeCikisFormu::on_pushButtonTutanakli_clicked);

    // Stok verilerini yükle
    //stokVerileriniYukle(); // Bu metot artık setProjeYolu veya setProjeAdi'nden sonra çağrılacak
    //urunAdlariYukle();
}

MalzemeCikisFormu::~MalzemeCikisFormu()
{
    delete ui;
}

void MalzemeCikisFormu::setProjeYolu(const QString &yol)
{
    if (yol.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje yolu boş olamaz!");
        return;
    }

    projeYolu = QDir::cleanPath(yol);
    //cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt"); // projeAdi henüz ayarlanmamış olabilir
    //stokDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_stok.txt");
    qDebug() << "setProjeYolu - projeYolu: " << projeYolu;
    // Hata ayıklama
    stokVerileriniYukle(); // stokVerileriniYukle'yi burada çağır
    urunAdlariYukle();
}

// Proje bilgilerini ayarlama
void MalzemeCikisFormu::setProjeAdi(const QString &adi)
{
    if (adi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje adı boş olamaz!");
        return;
    }

    projeAdi = adi;
    //cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    //stokDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_stok.txt");
    qDebug() << "setProjeAdi - projeAdi: " << projeAdi;
    // Hata ayıklama
    stokVerileriniYukle(); // stokVerileriniYukle'yi burada çağır
    urunAdlariYukle();
}

void MalzemeCikisFormu::stokVerileriniYukle()
{
    stokMap.clear(); // Eski verileri temizle

    if (projeYolu.isEmpty() || projeAdi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje adı veya yolu ayarlanmamış! Lütfen proje bilgilerini kontrol edin.");
        return;
    }

    cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    stokDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_stok.txt");

    qDebug() << "stokDosyaAdi: " << stokDosyaAdi; // Hata ayıklama

    QFile file(stokDosyaAdi);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Dosya Hatası", "Stok dosyası bulunamadı: " + stokDosyaAdi + "\n" + file.errorString());
        // Hata ayıklama
        return;
    }

    QTextStream in(&file);
    bool ilkSatir = true;
    while (!in.atEnd()) {
        QString satir = in.readLine().trimmed();
        if (ilkSatir) {
            ilkSatir = false;
            continue;
        }
        QStringList parcalar = satir.split(",");
        if (parcalar.size() >= 4) {
            QString urunAdi = parcalar[0].trimmed();
            int stokMiktari = parcalar[3].toInt();
            stokMap[urunAdi] = stokMiktari;
        }
    }

    file.close();
}

// Ürün adlarını yükle
void MalzemeCikisFormu::urunAdlariYukle()
{
    if (projeYolu.isEmpty() || projeAdi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje adı veya yolu ayarlanmamış! Lütfen proje bilgilerini kontrol edin.");
        return;
    }
    cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    stokDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_stok.txt");
    urunAdlariListesi = stokMap.keys();
}

// Kaydet butonu
void MalzemeCikisFormu::on_pushButtonKaydet2_clicked()
{
    stokVerileriniYukle();
    QString urun = ui->lineEdit_5->text().trimmed();
    int miktar = ui->lineEdit_6->text().toInt();
    QString tarih = ui->lineEdit_7->text().trimmed();
    if (urun.isEmpty() || miktar <= 0 || tarih.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Lütfen tüm alanları doldurun.");
        return;
    }

    if (!stokMap.contains(urun)) {
        QMessageBox::warning(this, "Hata", "Stokta böyle bir ürün yok.");
        return;
    }

    int mevcutStok = stokMap.value(urun);
    if (mevcutStok < miktar) {
        QMessageBox::warning(this, "Stok Yetersiz", "Yeterli stok yok.");
        return;
    }

    stokMap[urun] -= miktar;

    QFile file(cikisDosyaAdi);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << urun << "," << miktar << "," << tarih << "\n";
    }

    QFile stokFile(stokDosyaAdi);
    if (stokFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&stokFile);
        for (const auto &urun : stokMap.keys()) {
            out << urun << "," << stokMap.value(urun) << "\n";
        }
    }

    // TableWidget'a ekleme
    int satir = ui->tableWidget_cikislar->rowCount();
    ui->tableWidget_cikislar->insertRow(satir);
    ui->tableWidget_cikislar->setItem(satir, 0, new QTableWidgetItem(tarih));
    ui->tableWidget_cikislar->setItem(satir, 1, new QTableWidgetItem(urun));
    ui->tableWidget_cikislar->setItem(satir, 2, new QTableWidgetItem(QString::number(miktar)));
    ui->tableWidget_cikislar->setItem(satir, 3, new QTableWidgetItem("adet")); // Varsayılan birim

    // LineEdit'leri temizleme ve odaklanma
    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
    ui->lineEdit_7->clear();
    ui->lineEdit_5->setFocus();
    QMessageBox::information(this, "Başarılı", "Malzeme çıkışı kaydedildi.");
}

// Stok durumu butonu
void MalzemeCikisFormu::on_pushButtonStokDurumu2_clicked()
{
    auto *stokFormu = new MalzemeStokFormu(this);
    stokFormu->setProjeYolu(projeYolu);
    stokFormu->setProjeAdi(projeAdi);
    stokFormu->exec();
    delete stokFormu;
}
// Öneri listesi
void MalzemeCikisFormu::on_lineEdit_5_textChanged(const QString &text)
{
    onerilerListesi->clear();
    if (text.isEmpty()) {
        onerilerListesi->hide();
        return;
    }

    for (const QString &urun : urunAdlariListesi) {
        if (urun.contains(text, Qt::CaseInsensitive)) {
            onerilerListesi->addItem(urun);
        }
    }

    if (onerilerListesi->count() > 0) {
        QPoint pos = ui->lineEdit_5->mapTo(this, QPoint(0, ui->lineEdit_5->height()));
        onerilerListesi->move(pos);

        onerilerListesi->setStyleSheet("border: 1px solid gray; background-color: white; font-size: 12px;");
        onerilerListesi->show();
    } else {
        onerilerListesi->hide();
    }
}

// Öneri listesinden seçim
void MalzemeCikisFormu::onerilerdenSecildi(QListWidgetItem *item)
{
    ui->lineEdit_5->setText(item->text());
    onerilerListesi->hide();
}

// Event Filter
bool MalzemeCikisFormu::eventFilter(QObject *obj, QEvent *event)
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

// Tutanak butonu
void MalzemeCikisFormu::on_pushButtonTutanakli_clicked()
{
    Tutanak *tutanakPenceresi = new Tutanak(this);
    tutanakPenceresi->setProjeYolu(projeYolu);
    tutanakPenceresi->setProjeAdi(projeAdi);
    tutanakPenceresi->setModal(true);
    tutanakPenceresi->show();
    qDebug() << "Tutanak formuna aktarılan projeYolu: " << projeYolu;
    qDebug() << "Tutanak formuna aktarılan projeAdi: " << projeAdi;
}
