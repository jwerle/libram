#include <ras/ras.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "ram/ram.h"

static unsigned char *
ram_page(ram_t *ram, unsigned int i, unsigned int upsert) {
  unsigned char *page = ram->buffers[i];

  if (0 != page && 1 == upsert) {
    ras_free(page);
    ram->buffers[i] = 0;
    page = 0;
  }

  if (0 == page) {
    page = ras_alloc(ram->page_size);
    memset(page, 0, ram->page_size);
  }

  ram->buffers[i] = page;
  return page;
}

static void
ram_request_stat(ras_request_t *request) {
  ras_storage_stats_t stats = { 0 };
  ram_t *ram = (ram_t *) request->storage;
  stats.size = ram->length;

  request->callback(request, 0, &stats, sizeof(ras_storage_stats_t));
}

static void
ram_request_read(ras_request_t *request) {
  ram_t *ram = (ram_t *) request->storage;
  unsigned int i = request->offset / ram->page_size;
  unsigned int rel = request->offset - i * ram->page_size;
  unsigned int size = 0;
  unsigned int start = 0;

  if (request->offset + request->size > ram->length) {
    request->callback(request, ENOMEM, 0, 0);
    return;
  }

  while (start < request->size) {
    unsigned char *page = ram_page(ram, i++, 0);
    unsigned int avail = ram->page_size - rel;
    unsigned int want = request->size - start;
    unsigned int end = avail < want ? avail : want;

    if (0 != page) {
      // printf("read(i=%d, rel=%d, start=%d, end=%d)\n", i, rel, start, end);
      memcpy(request->data + start, page + rel, end);
    }

    start += end;
    size += end;
    rel = 0;
  }

  request->callback(request, 0, request->data, size);
}

static void
ram_request_write(ras_request_t *request) {
  ram_t *ram = (ram_t *) request->storage;
  unsigned int i = request->offset / ram->page_size;
  unsigned int rel = request->offset - i * ram->page_size;
  unsigned int start = 0;
  unsigned int length = request->offset + request->size;

  if (length > ram->length) {
    ram->length = length;
  }

  while (start < request->size) {
    unsigned char *page = ram_page(ram, i++, 1);
    unsigned int avail = ram->page_size - rel;
    unsigned int end = avail < (request->size - start)
      ? start + avail
      : request->size;

    memcpy(page + rel, request->data + start, end - start);
    start = end;
    rel = 0;
  }

  request->callback(request, 0, 0, 0);
}

static void
ram_request_del(ras_request_t *request) {
  ram_t *ram = (ram_t *) request->storage;
  unsigned int i = request->offset / ram->page_size;
  unsigned int rel = request->offset - i * ram->page_size;
  unsigned int start = 0;

  if (request->offset + request->size > ram->length) {
    request->size = (unsigned long int) fmax(
      0.0,
      (double) ram->length - (double) request->offset);
  }

  while (start < request->size) {
    if (0 == rel && request->size - start >= ram->page_size) {
      ras_free(ram->buffers[i]);
      ram->buffers[i++] = 0;
    } else {
      unsigned char *page = ram_page(ram, i++, 0);
      unsigned int avail = ram->page_size - rel;
      unsigned int end = avail < (request->size - start)
        ? start + avail
        : request->size;
      // printf("delete(i=%d, rel=%d, start=%d, end=%d)\n", i, rel, start, end);
      memset(page + rel, 0, end - start);
    }

    start += ram->page_size - rel;
    rel = 0;
  }

  if (request->offset + request->size >= ram->length) {
    ram->length = request->offset;
  }

  request->callback(request, 0, 0, 0);
}

static void
ram_request_destroy(ras_request_t *request) {
  ram_t *ram = (ram_t *) request->storage;

  for (int i = 0; i < RAM_MAX_BUFFERS; ++i) {
    if (0 != ram && 0 != ram->buffers[i]) {
      ras_free(ram->buffers[i]);
      ram->buffers[i] = 0;
    }
  }

  request->callback(request, 0, 0, 0);
}

int
ram_destroy(
  ras_storage_t *ram,
  ras_storage_destroy_callback_t *callback
) {
  return ras_storage_destroy(
    (ras_storage_t *) ram,
    (ras_storage_destroy_callback_t *) callback);
}

int
ram_stat(
  ras_storage_t *ram,
  ras_storage_stat_callback_t *callback) {
  return ras_storage_stat(
    (ras_storage_t *) ram,
    (ras_storage_stat_callback_t *) callback);
}

int
ram_delete(
  ras_storage_t *ram,
  unsigned long int offset,
  unsigned long int size,
  ras_storage_delete_callback_t *callback
) {
  return ras_storage_delete(
    (ras_storage_t *) ram,
    offset,
    size,
    (ras_storage_delete_callback_t *) callback);
}

int
ram_read(
  ras_storage_t *ram,
  unsigned long int offset,
  unsigned long int size,
  ras_storage_read_callback_t *callback
) {
  return ras_storage_read(
    (ras_storage_t *) ram,
    offset,
    size,
    (ras_storage_read_callback_t *) callback);
}

int
ram_write(
  ras_storage_t *ram,
  unsigned long int offset,
  unsigned long int size,
  void *buffer,
  ras_storage_write_callback_t *callback
) {
  return ras_storage_write(
    ram,
    offset,
    size,
    buffer,
    (ras_storage_write_callback_t *) callback);
}

ram_t *
ram_alloc() {
  return ras_alloc(sizeof(ram_t));
}

ras_storage_t *
ram_new() {
  ram_t *ram = ram_alloc();
  memset(ram, 0, sizeof(*ram));
  ras_storage_init((ras_storage_t *) ram,
    (ras_storage_options_t) {
      .del = ram_request_del,
      .stat = ram_request_stat,
      .read = ram_request_read,
      .write = ram_request_write,
      .destroy = ram_request_destroy,
    });

  ram->alloc = 1;
  ram->page_size = 1024 * 1024; // 1mb page size
  //ram->page_size = 8; // smallest page size
  return (ras_storage_t *) ram;
}
