#ifndef MULTIWATCHER_H
#define MULTIWATCHER_H

#include <QFutureSynchronizer>
#include <QThread>

class MultiWatcher : public QThread {
  Q_OBJECT

 signals:
  void allDone();

 public:
  void run() override;
  QFutureSynchronizer _sync;
};

#endif  // MULTIWATCHER_H
