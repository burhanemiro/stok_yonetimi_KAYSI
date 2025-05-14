//MainWindow.h (Başlık Dosyası):
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MalzemeGirisFormu;
class MalzemeCikisFormu;
class MalzemeStokFormu;
class Tutanak;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QDialog* getForm(const QString &formAdi);
    void addForm(const QString &formAdi, QDialog *form);

signals:
    void stokFormuAcildi(); // MalzemeGirisFormu veya MalzemeCikisFormu'ndan
    void tutanakFormuAcildi(); // MalzemeCikisFormu'ndan
    void formlarOlusturuldu();

public slots: // private slots'ı public slots olarak değiştirdim, gerekirse private olarak kalabilir
    void yeniProje();
    void projeYukle();
    void projeyiKaydet();
    void cikisYap();
    void malzemeGirisi();
    void malzemeCikisi();
    void malzemeStok();
    void malzemeTutanak();
    void on_pushButton_giris_clicked();
    void on_pushButton_cikis_clicked();
    void on_pushButton_stok_clicked();
    void handleStokFormuAcildi();
    void handleTutanakFormuAcildi();
    void formlaraProjeYoluAta();
    void formlaraSinyalleriBagla();

private:
    Ui::MainWindow *ui;
    QMap<QString, QDialog*> formlar;
    QString projeYolu;
    QString projeAdi;
    MalzemeGirisFormu *malzemeGirisFormu;
    MalzemeCikisFormu *malzemeCikisFormu;
    MalzemeStokFormu *malzemeStokFormu;
    Tutanak *tutanakFormu;
};

#endif // MAINWINDOW_H
