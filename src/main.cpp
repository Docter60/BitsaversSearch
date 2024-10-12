#include "MyApp.h"
#include "Updater.h"

int main() {
  BitsaversSearch::Updater::GetInstance().UpdateIfNeeded();
  MyApp app;
  app.Run();

  return 0;
}
