#include <string>

#include "ncurses_display.h"
#include "system.h"

int main(int argc, char *argv[]) {
  int num_processes_displayed{10};  // How many processes shall be displayed?

  // Handling an optional argument.
  try {
    if (argc > 1) {
      num_processes_displayed = std::stoi(argv[1]);
    }
  } catch (...) {
    num_processes_displayed = 10;
  }

  if (num_processes_displayed < 10 || num_processes_displayed > 20) {
    num_processes_displayed = 10;
  }

  System system;
  NCursesDisplay::Display(system, num_processes_displayed);
}