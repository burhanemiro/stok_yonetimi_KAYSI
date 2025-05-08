#include "malzemegirisformu.h"
#include "ui_malzemegirisformu.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>
#include <QFile>
#include "malzemeStokFormu.h"
#include <QListWidget>
#include <QTextStream>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QDir>

MalzemeGirisFormu::MalzemeGirisFormu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MalzemeGirisFormu)
{
    ui->setupUi(this);
    ui->lineEdit_1->installEventFilter(this);
    ui->lineEdit_2->installEventFilter(this);
    ui->lineEdit_3->installEventFilter(this);
    ui->lineEdit_4->installEventFilter(this);

    setTabOrder(ui->lineEdit_1, ui->lineEdit_2);
    setTabOrder(ui->lineEdit_2, ui->lineEdit_3);
    setTabOrder(ui->lineEdit_3, ui->lineEdit_4);
    setTabOrder(ui->lineEdit_4, ui->pushButtonKaydet);

    onerilerListesi = new QListWidget(this);
    onerilerListesi->hide();
    connect(ui->pushButtonKaydet, &QPushButton::clicked, this, &MalzemeGirisFormu::on_pushButtonKaydet_clicked);
    connect(ui->pushButtonStokDurumu, &QPushButton::clicked, this, &MalzemeGirisFormu::on_pushButtonStokDurumu_clicked);
    connect(onerilerListesi, &QListWidget::itemClicked, this, &MalzemeGirisFormu::onerilerdenSecildi);
}

MalzemeGirisFormu::~MalzemeGirisFormu()
{
    delete ui;
}

void MalzemeGirisFormu::setProjeYolu(const QString &yol)
{
    if (yol.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje yolu boş olamaz!");
        return;
    }

    projeYolu = QDir::cleanPath(yol);
    qDebug() << "Proje Yolu Ayarlandı: " << projeYolu;
}

void MalzemeGirisFormu::setProjeAdi(const QString &adi)
{
    if (adi.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Proje adı boş olamaz!");
        return;
    }

    projeAdi = adi;
    girisDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_giris.txt");
    stokDosyaAdi = QDir(projeYolu).filePath(projeAdi + "_stok.txt");
    qDebug() << "Proje Adı: " << projeAdi;
    qDebug() << "Giriş Dosyası: " << girisDosyaAdi;

    urunAdlariYukle();
}

bool MalzemeGirisFormu::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            QWidget::focusNextChild();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void MalzemeGirisFormu::urunAdlariYukle()
{
    urunAdlariListesi.clear();
    QFile dosya(girisDosyaAdi);
    if (!dosya.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
        // Dosya yoksa uyarı gerekmez çünkü ilk defa açılıyor olabilir
    }

    QTextStream stream(&dosya);
    while (!stream.atEnd()) {
        QString satir = stream.readLine();
        QString urunAdi = satir.split(",").value(0).trimmed();
        if (!urunAdi.isEmpty() && !urunAdlariListesi.contains(urunAdi)) {
            urunAdlariListesi.append(urunAdi);
        }
    }

    dosya.close();
}

void MalzemeGirisFormu::on_lineEdit_1_textChanged(const QString &text)
{
    onerilerListesi->clear();
    if (text.isEmpty()) {
        onerilerListesi->hide();
        return;
    }

    for (const QString &urun : urunAdlariListesi) {
        if (urun.startsWith(text, Qt::CaseInsensitive)) {
            onerilerListesi->addItem(urun);
        }
    }

    if (onerilerListesi->count() > 0) {
        QPoint pos = ui->lineEdit_1->mapTo(this, QPoint(0, ui->lineEdit_1->height()));
        onerilerListesi->move(pos);

        onerilerListesi->setStyleSheet("border: 1px solid gray; background-color: white; font-size: 12px;");
        onerilerListesi->show();
    } else {
        onerilerListesi->hide();
    }
}

void MalzemeGirisFormu::on_pushButtonKaydet_clicked()
{
    if (projeAdi.isEmpty() || projeYolu.isEmpty()) {
        QMessageBox::critical(this, "Hata", "Proje bilgileri ayarlanmamış!");
        return;
    }

    QString veri1 = ui->lineEdit_1->text().trimmed();
    QString veri2 = ui->lineEdit_2->text().trimmed();
    QString veri3 = ui->lineEdit_3->text().trimmed();
    QString veri4 = ui->lineEdit_4->text().trimmed();

    if (veri1.isEmpty() || veri2.isEmpty() || veri3.isEmpty() || veri4.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Lütfen tüm alanları doldurun!");
        return;
    }

    QFile file(girisDosyaAdi);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QMessageBox::critical(this, "Hata", "Dosya açılamadı: " + file.errorString());
        return;
    }

    QTextStream out(&file);
    out << veri1 << "," << veri2 << "," << veri3 << "," << veri4 << "\n";
    file.close();

    int satir = ui->tableWidget_Girisler->rowCount();
    ui->tableWidget_Girisler->insertRow(satir);
    ui->tableWidget_Girisler->setItem(satir, 0, new QTableWidgetItem(veri1));
    ui->tableWidget_Girisler->setItem(satir, 1, new QTableWidgetItem(veri2));
    ui->tableWidget_Girisler->setItem(satir, 2, new QTableWidgetItem(veri3));
    ui->tableWidget_Girisler->setItem(satir, 3, new QTableWidgetItem(veri4));

    ui->lineEdit_1->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_1->setFocus();

    QMessageBox::information(this, "Başarılı", "Veriler başarıyla kaydedildi!");
}

void MalzemeGirisFormu::onerilerdenSecildi(QListWidgetItem *item)
{
    ui->lineEdit_1->setText(item->text());
    onerilerListesi->hide();
}

void MalzemeGirisFormu::on_pushButtonStokDurumu_clicked()
{
    MalzemeStokFormu *stokPenceresi = new MalzemeStokFormu(this);
    stokPenceresi->setProjeAdi(projeAdi);
    stokPenceresi->setProjeYolu(projeYolu);
    stokPenceresi->setModal(true);
    stokPenceresi->exec();
}
