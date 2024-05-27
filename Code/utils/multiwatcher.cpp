#include "multiwatcher.h"

void MultiWatcher::run() {
  _sync.waitForFinished();
  emit allDone();
}
