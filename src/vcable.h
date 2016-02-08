#ifndef __vcable__
#define __vcable__

#include <vcable/plugin.h>

#define VCABLE_MAX_PLUGINS 32 // 32 is enough for everyone

struct vcable {
   void *handle[VCABLE_MAX_PLUGINS]; // dlopen handles for plugins
   struct vcable_plugin plugin[VCABLE_MAX_PLUGINS];
   struct vcable_plugin *active;
   struct vcable_options options;
};

void vcable_set_options(struct vcable *vcable, const struct vcable_options *options); // should be called once before vcable_set_plugin
bool vcable_set_plugin(struct vcable *vcable, uint32_t index); // index is offset by 1, 0 == OFF, returns true on success, false on failure
void vcable_write(struct vcable *vcable, size_t port, const vcable_sample *buffer, size_t num_samples, uint32_t sample_rate);
bool vcable_init(struct vcable *vcable); // returns true on success, false on failure
void vcable_release(struct vcable *vcable);

#endif /* __vcable__ */
