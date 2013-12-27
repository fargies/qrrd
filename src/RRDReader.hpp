#ifndef RRDREADER_H
#define RRDREADER_H

#include <QObject>

class RRDReader : public QObject
{
  Q_OBJECT
public:
  explicit RRDReader(
      const QString &file,
      QObject *parent = 0);

  QDateTime lastUpdate() const;
  QStringList dsNames() const;

  RRDInfo info() const;


signals:
  
public slots:
  
};

#endif // RRDREADER_H
