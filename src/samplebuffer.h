#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

struct samplebuffer {
   uint8_t *buffer;
   size_t off, size;
};

static inline void
samplebuffer_write(struct samplebuffer *buf, const void *data, size_t size)
{
   assert(buf && buf->buffer && data);

   // If we have filled the buffer, it's better to give up.
   if (buf->off + size > buf->size)
      return;

   memcpy(buf->buffer + buf->off, data, size);
   buf->off += size;
   assert(buf->off <= buf->size);
}

static inline void
samplebuffer_read(struct samplebuffer *buf, void *dst, size_t size)
{
   assert(buf && buf->buffer);
   const size_t r = (size > buf->off ? buf->off : size);
   memcpy(dst, buf->buffer, r);
   assert(r <= buf->size);
   memmove(buf->buffer, buf->buffer + r, buf->size - r);
   assert(buf->off >= r);
   buf->off -= r;
}

static inline bool
samplebuffer(struct samplebuffer *buf, size_t nmemb, size_t size)
{
   *buf = (struct samplebuffer){0};

   // Make nmemb at least 4096 to handle differences between host / plugin at the cost of extra latency.
   nmemb = (nmemb > 4096 ? nmemb : 4096);
   if (!(buf->buffer = calloc(nmemb, size)))
      return false;

   buf->size = nmemb * size;
   return true;
}

static inline void
samplebuffer_release(struct samplebuffer *buf)
{
   if (!buf)
      return;

   if (buf->buffer)
      free(buf->buffer);

   *buf = (struct samplebuffer){0};
}
