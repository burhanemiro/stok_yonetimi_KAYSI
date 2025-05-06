#include "malzemecikisformu.h"
#include "ui_malzemecikisformu.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include "malzemeStokFormu.h"
#include <QListWidget>
#include "tutanak.h"
#include <QFileDialog>

MalzemeCikisFormu::MalzemeCikisFormu(QWidget *parent, const QString &yol, const QString &adi) :
    QDialog(parent),
    ui(new Ui::MalzemeCikisFormu),
    projeAdi(adi),
    projeYolu(yol),
    malzemeStokFormu(nullptr),
    tutanak(nullptr)
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
    stokVerileriniYukle();
    urunAdlariYukle();
}

MalzemeCikisFormu::~MalzemeCikisFormu()
{
    delete ui;

    // Stok ve tutanak pencerelerini kapat ve sil
    if (malzemeStokFormu != nullptr) {
        delete malzemeStokFormu;
        malzemeStokFormu = nullptr; // Eski referansı sıfırla
    }
    if (tutanak != nullptr) {
        delete tutanak;
        tutanak = nullptr; // Eski referansı sıfırla
    }
}

void MalzemeCikisFormu::setProjeYolu(const QString &yol)
{
    projeYolu = yol;
    guncelleDosyaYollari();
}

void MalzemeCikisFormu::setProjeAdi(const QString &adi)
{
    projeAdi = adi;
    guncelleDosyaYollari();
}

void MalzemeCikisFormu::guncelleDosyaYollari()
{
    cikisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_cikis.txt");
    stokDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_stok.txt");
    qDebug() << "Dosya Yolları Güncellendi - stokDosyaAdi: " << stokDosyaAdi;
}

void MalzemeCikisFormu::stokVerileriniYukle()
{
    stokMap.clear(); // Eski verileri temizle

    if (projeYolu.isEmpty() || projeAdi.isEmpty()) {
        qDebug() << "Hata: Proje adı veya yolu ayarlanmamış!";
        return;
    }

    qDebug() << "stokDosyaAdi: " << stokDosyaAdi;

    QFile file(stokDosyaAdi);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Dosya Hatası: Stok dosyası bulunamadı: " << stokDosyaAdi << " Hata: " << file.errorString();
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
    qDebug() << "Stok verileri başarıyla yüklendi. Ürün sayısı: " << stokMap.size();
}

void MalzemeCikisFormu::urunAdlariYukle()
{
    urunAdlariListesi = stokMap.keys();
}

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
            out << urun << "," << stokMap[urun] << "\n";
        }
    }

    int satir = ui->tableWidget_cikislar->rowCount();
    ui->tableWidget_cikislar->insertRow(satir);
    ui->tableWidget_cikislar->setItem(satir, 0, new QTableWidgetItem(tarih));
    ui->tableWidget_cikislar->setItem(satir, 1, new QTableWidgetItem(urun));
    ui->tableWidget_cikislar->setItem(satir, 2, new QTableWidgetItem(QString::number(miktar)));
    ui->tableWidget_cikislar->setItem(satir, 3, new QTableWidgetItem("adet"));

    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
    ui->lineEdit_7->clear();
    ui->lineEdit_5->setFocus();

    QMessageBox::information(this, "Başarılı", "Malzeme çıkışı kaydedildi.");
}

void MalzemeCikisFormu::on_pushButtonStokDurumu2_clicked()
{
    if (malzemeStokFormu == nullptr || !malzemeStokFormu->isVisible()) {
        malzemeStokFormu = new MalzemeStokFormu(this);
        malzemeStokFormu->setProjeAdi(projeAdi);
        malzemeStokFormu->setProjeYolu(projeYolu);
        malzemeStokFormu->setAttribute(Qt::WA_DeleteOnClose);
        malzemeStokFormu->show();
    } else {
        malzemeStokFormu->raise();
        malzemeStokFormu->activateWindow();
    }
}

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

        onerilerListesi->setStyleSheet("border: 1px solid gray; background-color: white; font-size: 14px;");
        onerilerListesi->show();
    } else {
        onerilerListesi->hide();
    }
}

void MalzemeCikisFormu::onerilerdenSecildi(QListWidgetItem *item)
{
    ui->lineEdit_5->setText(item->text());
    onerilerListesi->hide();
}

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

void MalzemeCikisFormu::on_pushButtonTutanakli_clicked()
{
    if (tutanak == nullptr || !tutanak->isVisible()) {
        tutanak = new Tutanak(this, projeAdi, projeYolu);
        tutanak->setAttribute(Qt::WA_DeleteOnClose);
        tutanak->show();
    } else {
        tutanak->raise();
        tutanak->activateWindow();
    }

    qDebug() << "Tutanak formuna aktarılan projeAdi: " << projeAdi;
    qDebug() << "Tutanak formuna aktarılan projeYolu: " << projeYolu;
}
