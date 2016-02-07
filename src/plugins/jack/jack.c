#include <vcable/plugin.h>
#include <jack/jack.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef jack_default_audio_sample_t jack_sample;

#define MAX_PORTS 256

struct jack {
   jack_sample *inbuf[MAX_PORTS];
   jack_client_t *client;
   jack_port_t *input[MAX_PORTS];
   jack_port_t *output[MAX_PORTS];
   struct vcable_options options;
   size_t max_samples;
};

static void
release_buffers(struct jack *jack)
{
   assert(jack);
   for (size_t i = 0; i < jack->options.ports; ++i) {
      free(jack->inbuf[i]);
      jack->inbuf[i] = NULL;
   }
}

static int
bufsz_cb(jack_nframes_t nframes, void *arg)
{
   struct jack *jack = arg;
   assert(jack);

   jack->max_samples = nframes;
   release_buffers(jack);

   for (size_t i = 0; i < jack->options.ports; ++i) {
      if (!(jack->inbuf[i] = calloc(sizeof(jack_sample), jack->max_samples))) {
         fprintf(stderr, "jack: calloc() failed for plugin <-> host buffer for port index %zu\n", i);
         goto fail;
      }
   }

   return 0;

fail:
   release_buffers(jack);
   return -1;
}

static void
jack_release(struct jack *jack)
{
   if (!jack)
      return;

   for (size_t i = 0; i < MAX_PORTS; ++i) {
      if (jack->input[i])
         jack_port_unregister(jack->client, jack->input[i]);
      if (jack->output[i])
         jack_port_unregister(jack->client, jack->output[i]);
      if (jack->inbuf[i])
         free(jack->inbuf[i]);
   }

   if (jack->client)
      jack_client_close(jack->client);

   *jack = (struct jack){0};
}

static bool
jack_init(struct jack *jack, int (*process_cb)(jack_nframes_t, void*), const struct vcable_options *options)
{
   assert(jack && options && options->write_cb);
   *jack = (struct jack){0};
   jack->options = *options;

   if (sizeof(jack_sample) != options->sample_size) {
      fprintf(stderr, "jack: sizeof(jack_sample) != sample_size can't proceed. (%u != %u)\n", (uint8_t)sizeof(jack_sample), options->sample_size);
      goto fail;
   }

   if (options->ports > MAX_PORTS) {
      fprintf(stderr, "jack: requested too many ports (%zu requested, %u maximum)\n", options->ports, MAX_PORTS);
      goto fail;
   }

   if (!(jack->client = jack_client_open(options->name, JackNoStartServer, NULL))) {
      fprintf(stderr, "jack: jack_client_open() failed\n");
      goto fail;
   }

   if (jack_set_process_callback(jack->client, process_cb, jack) != 0) {
      fprintf(stderr, "jack: jack_set_process_callback() failed\n");
      goto fail;
   }

   if (jack_set_buffer_size_callback(jack->client, bufsz_cb, jack) != 0) {
      fprintf(stderr, "jack: jack_set_buffer_size_callback() failed\n");
      goto fail;
   }

   if (!(jack->max_samples = jack_get_buffer_size(jack->client))) {
      fprintf(stderr, "jack: jack_get_buffer_size() returned <= 0 value %zu\n", jack->max_samples);
      goto fail;
   }

   if (jack_activate(jack->client) != 0) {
      fprintf(stderr, "jack: jack_activate() failed\n");
      goto fail;
   }

   for (size_t i = 0; i < jack->options.ports; ++i) {
      char name[256];
      snprintf(name, sizeof(name), "input%zu", i);
      if (!(jack->input[i] = jack_port_register(jack->client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0))) {
         fprintf(stderr, "jack: jack_port_register() failed for input port %s\n", name);
         goto fail;
      }

      snprintf(name, sizeof(name), "output%zu", i);
      if (!(jack->output[i] = jack_port_register(jack->client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0))) {
         fprintf(stderr, "jack: jack_port_register() failed for output port %s\n", name);
         goto fail;
      }

      jack_connect(jack->client, jack_port_name(jack->output[i]), jack_port_name(jack->input[i]));
   }

   if (bufsz_cb(jack->max_samples, jack) != 0)
      goto fail;

   return true;

fail:
   jack_release(jack);
   return false;
}

static struct jack jack;

static void
write_cb(size_t port, const vcable_sample *data, size_t num_samples, uint32_t sample_rate)
{
   if (port >= MAX_PORTS || sample_rate != jack_get_sample_rate(jack.client))
      return;

   assert(jack.inbuf[port]);
   const uint32_t w = (num_samples > jack.max_samples ? jack.max_samples : num_samples);
   assert(w <= jack.max_samples);
   memcpy(jack.inbuf[port], data, w * sizeof(jack_sample));
   memset(jack.inbuf[port] + w, 0, (jack.max_samples - w) * sizeof(jack_sample));
}

static int
process_cb(jack_nframes_t nframes, void *arg)
{
   (void)arg;
   assert(jack.options.write_cb);
   assert(nframes <= jack.max_samples);

   const size_t bufsz = nframes * sizeof(jack_sample);
   for (size_t i = 0; i < jack.options.ports; ++i) {
      const jack_sample *in = jack_port_get_buffer(jack.input[i], nframes);
      jack_sample *out = jack_port_get_buffer(jack.output[i], nframes);
      assert(in && out && jack.inbuf[i]);
      jack.options.write_cb(i, in, nframes, jack_get_sample_rate(jack.client), jack.options.userdata);
      memcpy(out, jack.inbuf[i], bufsz);
      memset(jack.inbuf[i], 0, bufsz);
   }

   return 0;
}

static bool
open(const struct vcable_options *options)
{
   jack_release(&jack);
   return jack_init(&jack, process_cb, options);
}

static void
close(void)
{
   jack_release(&jack);
}

#pragma GCC diagnostic ignored "-Wmissing-prototypes"

void
plugin_register(struct vcable_plugin *out_plugin)
{
   out_plugin->name = "jack";
   out_plugin->description = "Provides audio input ports as JACK client";
   out_plugin->version = VCABLE_PLUGIN_VERSION;
   out_plugin->open = open;
   out_plugin->close = close;
   out_plugin->write = write_cb;
}
