
#include "fildesh.h"
#include <assert.h>

bool sxproto2textproto(FildeshX* in, FildeshO* out);


int main(int argc, char** argv)
{
  FildeshX* in = NULL;
  FildeshO* out = open_fd_FildeshO(1);
  int exstatus = 0;

  assert(argc == 2);
  in = open_FildeshXF(argv[1]);
  assert(in);

  if (!sxproto2textproto(in, out)) {
    exstatus = 1;
  }
  close_FildeshO(out);
  return exstatus;
}
