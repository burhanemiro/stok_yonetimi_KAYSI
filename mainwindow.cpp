#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "malzemegirisformu.h"
#include "malzemecikisformu.h"
#include "malzemestokformu.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QDir>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , malzemeGirisFormu(nullptr)
    , malzemeCikisFormu(nullptr)
    , malzemeStokFormu(nullptr)
    , tutanak(nullptr)
{
    ui->setupUi(this);

    // Menü eylemleri
    connect(ui->actionYeni_proje, &QAction::triggered, this, &MainWindow::yeniProje);
    connect(ui->actionProje_yukle, &QAction::triggered, this, &MainWindow::projeYukle);
    connect(ui->actionProjeyi_kaydet, &QAction::triggered, this, &MainWindow::projeyiKaydet);
    connect(ui->action_cik, &QAction::triggered, this, &MainWindow::cikisYap);

    connect(ui->actionMalzeme_giris, &QAction::triggered, this, &MainWindow::malzemeGirisi);
    connect(ui->actionMalzeme_cikis, &QAction::triggered, this, &MainWindow::malzemeCikisi);
    connect(ui->actionStok_durumu, &QAction::triggered, this, &MainWindow::malzemeStok);
    connect(ui->actionTutanak_olustur, &QAction::triggered, this, &MainWindow::malzemeTutanak);

    // Buton eylemleri
    connect(ui->pushButton_giris, &QPushButton::clicked, this, &MainWindow::on_pushButton_giris_clicked);
    connect(ui->pushButton_cikis, &QPushButton::clicked, this, &MainWindow::on_pushButton_cikis_clicked);
    connect(ui->pushButton_stok, &QPushButton::clicked, this, &MainWindow::on_pushButton_stok_clicked);

    this->showMaximized(); // Tam ekran başlat
}

MainWindow::~MainWindow()
{
    delete ui;

    delete malzemeGirisFormu;
    delete malzemeCikisFormu;
    delete tutanak;
}

void MainWindow::yeniProje()
{
    QString dosyaYolu = QFileDialog::getSaveFileName(this, tr("Yeni Proje Oluştur"),
                                                     QDir::homePath(),
                                                     tr("Proje Dosyası (*.proje);;Tüm Dosyalar (*)"));

    if (dosyaYolu.isEmpty()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Bir proje adı girilmedi!"));
        return;
    }

    if (!dosyaYolu.endsWith(".proje"))
        dosyaYolu += ".proje";

    aktifProjeYolu = QDir::cleanPath(QFileInfo(dosyaYolu).absolutePath());
    aktifProjeAdi = QFileInfo(dosyaYolu).baseName();

    QFile projeDosyasi(dosyaYolu);
    if (!projeDosyasi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Hata"), tr("Proje dosyası oluşturulamadı: ") + projeDosyasi.errorString());
        return;
    }

    QTextStream out(&projeDosyasi);
    out << "Proje Adı: " << aktifProjeAdi << "\n";
    out << "Oluşturma Tarihi: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    out << "Proje Yolu: " << aktifProjeYolu << "\n";
    projeDosyasi.close();

    QMessageBox::information(this, tr("Başarılı"), tr("Proje başarıyla oluşturuldu: ") + aktifProjeYolu);

    formlaraProjeYoluAta();
}

void MainWindow::projeYukle()
{
    QString dosyaYolu = QFileDialog::getOpenFileName(this, tr("Proje Yükle"),
                                                     QDir::homePath(),
                                                     tr("Proje Dosyası (*.proje);;Tüm Dosyalar (*)"));

    if (dosyaYolu.isEmpty()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Bir proje seçilmedi!"));
        return;
    }

    aktifProjeYolu = QDir::cleanPath(QFileInfo(dosyaYolu).absolutePath());
    aktifProjeAdi = QFileInfo(dosyaYolu).baseName();

    QMessageBox::information(this, tr("Başarılı"), tr("Proje başarıyla yüklendi: ") + aktifProjeYolu);

    formlaraProjeYoluAta();
}

void MainWindow::projeyiKaydet()
{
    if (aktifProjeYolu.isEmpty()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Kayıt için bir proje yüklenmedi veya oluşturulmadı!"));
        return;
    }

    QString dosyaYolu = QFileDialog::getSaveFileName(this, tr("Projeyi Kaydet"),
                                                     aktifProjeYolu,
                                                     tr("Proje Dosyası (*.proje);;Tüm Dosyalar (*)"));

    if (dosyaYolu.isEmpty()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Bir dosya adı girilmedi!"));
        return;
    }

    if (!dosyaYolu.endsWith(".proje"))
        dosyaYolu += ".proje";

    QFile dosya(dosyaYolu);
    if (!dosya.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Hata"), tr("Dosya açılamadı: ") + dosya.errorString());
        return;
    }

    QTextStream out(&dosya);
    out << "Proje Yolu: " << aktifProjeYolu << "\n";
    dosya.close();

    QMessageBox::information(this, tr("Başarılı"), tr("Proje başarıyla kaydedildi: ") + dosyaYolu);
}

void MainWindow::cikisYap()
{
    QApplication::quit();
}

void MainWindow::on_pushButton_giris_clicked()
{
    malzemeGirisi();
}

void MainWindow::on_pushButton_cikis_clicked()
{
    malzemeCikisi();
}

void MainWindow::on_pushButton_stok_clicked()
{
    malzemeStok();
}

void MainWindow::malzemeGirisi()
{
    if (!malzemeGirisFormu) {
        malzemeGirisFormu = new MalzemeGirisFormu(this);
        malzemeGirisFormu->setProjeYolu(aktifProjeYolu);
        malzemeGirisFormu->setProjeAdi(aktifProjeAdi);
    }

    malzemeGirisFormu->show();
}

void MainWindow::malzemeCikisi()
{
    if (!malzemeCikisFormu) {
        malzemeCikisFormu = new MalzemeCikisFormu(this,aktifProjeAdi, aktifProjeYolu);
        malzemeCikisFormu->setProjeYolu(aktifProjeYolu);
        malzemeCikisFormu->setProjeAdi(aktifProjeAdi);
    }

    malzemeCikisFormu->show();
}

void MainWindow::malzemeStok()
{
    if (!malzemeStokFormu) {
        malzemeStokFormu = new MalzemeStokFormu(this);
        malzemeStokFormu->setProjeYolu(aktifProjeYolu);
        malzemeStokFormu->setProjeAdi(aktifProjeAdi);
    }

    malzemeStokFormu->show();
}

void MainWindow::malzemeTutanak()
{
    qDebug() << "MainWindow::malzemeTutanak() çalıştı";
    qDebug() << "Aktarılacak projeAdi: " << aktifProjeAdi;
    qDebug() << "Aktarılacak projeYolu: " << aktifProjeYolu;

    if (aktifProjeAdi.isEmpty() || aktifProjeYolu.isEmpty()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Lütfen önce bir proje oluşturun veya yükleyin!"));
        return;
    }

    if (!tutanak) {
        tutanak = new Tutanak(this,aktifProjeAdi, aktifProjeYolu);
        tutanak->setProjeAdi(aktifProjeAdi);
        tutanak->setProjeYolu(aktifProjeYolu);
    }
    tutanak->setModal(true);
    tutanak->show();
    tutanak->raise();
    tutanak->activateWindow();
}

void MainWindow::formlaraProjeYoluAta()
{
    if (malzemeGirisFormu) {
        malzemeGirisFormu->setProjeYolu(aktifProjeYolu);
        malzemeGirisFormu->setProjeAdi(aktifProjeAdi);
    }

    if (malzemeCikisFormu) {
        malzemeCikisFormu->setProjeYolu(aktifProjeYolu);
        malzemeCikisFormu->setProjeAdi(aktifProjeAdi);
    }

    if (malzemeStokFormu) {
        malzemeStokFormu->setProjeYolu(aktifProjeYolu);
        malzemeStokFormu->setProjeAdi(aktifProjeAdi);
    }

    if (tutanak) {
        delete tutanak;
        tutanak = nullptr;
    }
}
