#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "malzemegirisformu.h"
#include "malzemecikisformu.h"
#include "malzemestokformu.h"
#include "tutanak.h" // Tutanak formunu dahil ettik
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setProjeYolu(const QString &yol);
    QString getAktifProjeAdi() const;
    void cikisYap();

private slots:
    void on_pushButton_giris_clicked();
    void on_pushButton_cikis_clicked();
    void on_pushButton_stok_clicked();

    void yeniProje();
    void projeYukle();
    void projeyiKaydet();

    void malzemeGirisi();
    void malzemeCikisi();
    void malzemeStok();
    void malzemeTutanak();

private:
    Ui::MainWindow *ui;

    MalzemeGirisFormu *malzemeGirisFormu;
    MalzemeCikisFormu *malzemeCikisFormu;
    MalzemeStokFormu *malzemeStokFormu;
    Tutanak *tutanak; // Tutanak formu için üye değişken
    QString aktifProjeYolu;
    QString aktifProjeAdi;

    void formlaraProjeYoluAta();
};

#endif // MAINWINDOW_H
