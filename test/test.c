#include <ras/ras.h>
#include <assert.h>
#include <string.h>
#include <ok/ok.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <ram/ram.h>

static void
onstat(ras_storage_t *ram, int err, ras_storage_stats_t *stats) {
  if (err > 0) {
    ok("onstat(err=%d type=%d): %s",
      err,
      ram->last_request.type,
      strerror(err));
  } else {
    ok("onstat(): size=%lu ", stats->size);
  }
}

static void
onwrite(ras_storage_t *ram, int err) {
  if (err > 0) {
    ok("onwrite(err=%d type=%d): %s",
      err,
      ram->last_request.type,
      strerror(err));
  }
}

static void
onread(
  ras_storage_t *ram,
  int err,
  void *buffer,
  unsigned long int size
) {
  if (err > 0) {
    ok("onread(err=%d type=%d): %s",
        err,
        ram->last_request.type,
        strerror(err));
  } else {
    unsigned char *data = buffer;
    for (int i = 0; i < size; ++i) {
      ok("%0.2x ", (unsigned char) data[i]);
    }
    ok("");
  }
}

static void
ondelete(
  ras_storage_t *ram,
  int err
) {
  if (err > 0) {
    ok("ondelete(err=%d type=%d): %s",
      err,
      ram->last_request.type,
      strerror(err));
  }
}

static void
ondestroy(
  ras_storage_t *ram,
  int err
) {
  if (err > 0) {
    ok("ondestroy(err=%d type=%d): %s",
      err,
      ram->last_request.type,
      strerror(err));
  }

  ok("destroyed!");
}

int
main(void) {
  ras_storage_t *ram = ram_new();
  unsigned char buffer[] = {
    0xde, 0xad, 0xfa, 0xce,
    0xfe, 0xfe, 0xfe, 0xee,
    0
  }; // >= 0-255

  ram_write(ram, 4096, 8, buffer, onwrite);
  ram_delete(ram, 4080, 19, ondelete);
  ram_read(ram, 4096, 8, onread);
  ram_stat(ram, onstat);
  ram_destroy(ram, ondestroy);

  const struct ras_allocator_stats_s stats = ras_allocator_stats();
  assert(stats.alloc == stats.free); // memory leak in ras allocator
  return 0;
}
