#ifndef TUTANAK_H
#define TUTANAK_H

#include <QDialog>

namespace Ui {
class Tutanak;
}

class Tutanak : public QDialog
{
    Q_OBJECT

public:
    explicit Tutanak(QWidget *parent = nullptr); // Constructor güncellendi
    ~Tutanak();

    void setProjeAdi(const QString &adi);
    void setProjeYolu(const QString &yol);

protected:
    // Event filter
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_pushButtonMek_clicked();
    void on_pushButtonKayit_clicked();
    void on_pushButtonYaz_clicked();

private:
    Ui::Tutanak *ui;

    QString projeAdi; // Kaldırıldı
    QString projeYolu; // Kaldırıldı
    QString cikisDosyaAdi;
    QString stokDosyaAdi;
};

#endif // TUTANAK_H
