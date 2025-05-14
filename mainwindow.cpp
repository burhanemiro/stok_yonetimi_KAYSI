#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "malzemegirisformu.h"
#include "malzemecikisformu.h"
#include "malzemestokformu.h"
#include "tutanak.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QDebug> // qDebug ekledik

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , malzemeGirisFormu(nullptr)
    , malzemeCikisFormu(nullptr)
    , malzemeStokFormu(nullptr)
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

    this->showMaximized();

    // Formları oluştur ve sinyalleri bağla
    formlaraSinyalleriBagla();
}

MainWindow::~MainWindow()
{
    delete ui;

    // Formları sil
    QMapIterator<QString, QDialog*> i(formlar);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }
    formlar.clear();
}

void MainWindow::formlaraSinyalleriBagla()
{
    malzemeGirisi();
    malzemeCikisi();
    malzemeStok();
    malzemeTutanak();

    MalzemeGirisFormu *girisForm = qobject_cast<MalzemeGirisFormu*>(getForm("malzemeGirisi"));
    if (girisForm) {
        disconnect(girisForm, &MalzemeGirisFormu::stokFormuAcildi, this, &MainWindow::handleStokFormuAcildi);
        connect(girisForm, &MalzemeGirisFormu::stokFormuAcildi, this, &MainWindow::handleStokFormuAcildi);
    }

    MalzemeCikisFormu *cikisForm = qobject_cast<MalzemeCikisFormu*>(getForm("malzemeCikisi"));
    if (cikisForm) {
        disconnect(cikisForm, &MalzemeCikisFormu::stokFormuAcildi, this, &MainWindow::handleStokFormuAcildi);
        connect(cikisForm, &MalzemeCikisFormu::stokFormuAcildi, this, &MainWindow::handleStokFormuAcildi);

        disconnect(cikisForm, &MalzemeCikisFormu::tutanakFormuAcildi, this, &MainWindow::handleTutanakFormuAcildi);
        connect(cikisForm, &MalzemeCikisFormu::tutanakFormuAcildi, this, &MainWindow::handleTutanakFormuAcildi);
    }
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

    projeYolu = QDir::cleanPath(QFileInfo(dosyaYolu).absolutePath());
    projeAdi = QFileInfo(dosyaYolu).baseName();

    QFile projeDosyasi(dosyaYolu);
    if (!projeDosyasi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Hata"), tr("Proje dosyası oluşturulamadı: ") + projeDosyasi.errorString());
        return;
    }

    QTextStream out(&projeDosyasi);
    out << "Proje Adı: " << projeAdi << "\n";
    out << "Oluşturma Tarihi: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    out << "Proje Yolu: " << projeYolu << "\n";
    projeDosyasi.close();
    QMessageBox::information(this, tr("Başarılı"), tr("Proje başarıyla oluşturuldu: ") + projeYolu);

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

    projeYolu = QDir::cleanPath(QFileInfo(dosyaYolu).absolutePath());
    projeAdi = QFileInfo(dosyaYolu).baseName();

    QMessageBox::information(this, tr("Başarılı"), tr("Proje başarıyla yüklendi: ") + projeYolu);

    formlaraProjeYoluAta();
}

void MainWindow::projeyiKaydet()
{
    if (projeYolu.isEmpty()) {
        QMessageBox::warning(this, tr("Uyarı"), tr("Kayıt için bir proje yüklenmedi veya oluşturulmadı!"));
        return;
    }

    QString dosyaYolu = QFileDialog::getSaveFileName(this, tr("Projeyi Kaydet"),
                                                     projeYolu,
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
    out << "Proje Yolu: " << projeYolu << "\n";
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
    MalzemeGirisFormu *form = qobject_cast<MalzemeGirisFormu*>(getForm("malzemeGirisi"));
    if (!form) {
        form = new MalzemeGirisFormu(this);
        form->setProjeYolu(projeYolu);
        form->setProjeAdi(projeAdi);
        addForm("malzemeGirisi", form);
    }
    form->show();
}

void MainWindow::malzemeCikisi()
{
    MalzemeCikisFormu *form = qobject_cast<MalzemeCikisFormu*>(getForm("malzemeCikisi"));
    if (!form) {
        form = new MalzemeCikisFormu(this);
        form->setProjeYolu(projeYolu);
        form->setProjeAdi(projeAdi);
        addForm("malzemeCikisi", form);
    }
    form->show();
}

void MainWindow::malzemeStok()
{
    MalzemeStokFormu *form = qobject_cast<MalzemeStokFormu*>(getForm("malzemeStok"));
    if (!form) {
        form = new MalzemeStokFormu(this);
        form->setProjeYolu(projeYolu);
        form->setProjeAdi(projeAdi);
        addForm("malzemeStok", form);
    }

    if (!form->isVisible()) {
        form->show();
        form->raise();
        form->activateWindow();
    }
}

void MainWindow::malzemeTutanak()
{
    Tutanak *form = qobject_cast<Tutanak*>(getForm("tutanak"));
    if (!form) {
        form = new Tutanak(this);
        if (projeAdi.isEmpty() || projeYolu.isEmpty()) {
            QMessageBox::warning(this, tr("Uyarı"), tr("Lütfen önce bir proje oluşturun veya yükleyin!"));
            delete form;
            return;
        }
        form->setProjeYolu(projeYolu);
        form->setProjeAdi(projeAdi);
        addForm("tutanak", form);
    }

    if (!form->isVisible()) {
        form->setModal(true);
        form->show();
        form->raise();
        form->activateWindow();
    }
}

void MainWindow::formlaraProjeYoluAta()
{
    QMapIterator<QString, QDialog*> i(formlar);
    while (i.hasNext()) {
        i.next();
        MalzemeGirisFormu *girisFormu = qobject_cast<MalzemeGirisFormu*>(i.value());
        MalzemeCikisFormu *cikisFormu = qobject_cast<MalzemeCikisFormu*>(i.value());
        MalzemeStokFormu *stokFormu = qobject_cast<MalzemeStokFormu*>(i.value());
        Tutanak *tutanakFormu = qobject_cast<Tutanak*>(i.value());
        if (girisFormu) {
            girisFormu->setProjeYolu(projeYolu);
            girisFormu->setProjeAdi(projeAdi);
        } else if (cikisFormu) {
            cikisFormu->setProjeYolu(projeYolu);
            cikisFormu->setProjeAdi(projeAdi);
        } else if (stokFormu) {
            stokFormu->setProjeYolu(projeYolu);
            stokFormu->setProjeAdi(projeAdi);
        } else if (tutanakFormu) {
            tutanakFormu->setProjeYolu(projeYolu);
            tutanakFormu->setProjeAdi(projeAdi);
        }
    }
}

QDialog* MainWindow::getForm(const QString &formAdi)
{
    return formlar.value(formAdi, nullptr);
}

void MainWindow::addForm(const QString &formAdi, QDialog *form)
{
    formlar.insert(formAdi, form);
}

void MainWindow::handleStokFormuAcildi()
{
    static bool isHandlingStokForm = false;

    if (isHandlingStokForm) {
        return; // Eğer işlem zaten yapılıyorsa, tekrar işlem yapma
    }

    isHandlingStokForm = true;
    malzemeStok();
    isHandlingStokForm = false;
}

void MainWindow::handleTutanakFormuAcildi()
{
    static bool isHandlingTutanakForm = false;
    if (isHandlingTutanakForm) return;

    isHandlingTutanakForm = true;
    malzemeTutanak();
    isHandlingTutanakForm = false;
}
