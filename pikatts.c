/*
 * Copyright (C) 2023 Silent Twin
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pikatts.h"
#include "picoapi.h"
#include "picoapid.h"
#include "picoos.h"
#include "picorsrc.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const pika_Error pika_OK = {.system = NULL, .message = "", .status = 0};

_Atomic(unsigned long int) pika_idCounter = 0;

/**
 * pika_newVoiceName generates a new unique voice name based on the given name
 * @param name: prefix of the generated name
 */
char *pika_newVoiceName(const char *name) {
  // len + max uint32 + \0
  int cap = strlen(name) + 10 + 1;
  char *buf = calloc(cap, 1);
  snprintf(buf, cap, "%s#%lu", name, ++pika_idCounter);
  return buf;
}

/**
 * pika_err constructs a new pika_Error
 * @param ctx: the pika context
 * @param message: contextual details about the error
 * @param status: the pico status code
 */
pika_Error pika_err(const pika_Context *ctx, const char *message,
                    const int status) {
  if (status == 0) {
    return pika_OK;
  }

  pika_Error err = {
      .system = ctx != NULL && ctx->system != NULL ? ctx->system : NULL,
      .message = message,
      .status = status,
  };
  return err;
}

/**
 * pika_error_message generates an error message based on err
 * @param err: the pika error
 */
char *pika_error_message(pika_Error err) {
  char *buf = calloc(PICO_RETSTRINGSIZE + 3 + strlen(err.message), 1);
  strcpy(buf, err.message);

  if (err.system == NULL || err.status == 0) {
    return buf;
  }

  strcat(buf, ": ");
  pico_getSystemStatusMessage(err.system, err.status, buf + strlen(buf));
  return buf;
}

/**
 * pika_loadResource loads the pico resource file specified by fn
 * @param ctx: the pika context
 * @param fn: location of the resource file
 * @param outRes: pointer to store the pico resource
 */
pika_Error pika_loadResource(const pika_Context *ctx, const char *fn,
                             pico_Resource *outRes) {
  int status = pico_loadResource(ctx->system, (pico_Char *)fn, outRes);
  if (status != 0) {
    return pika_err(ctx, fn, status);
  }

  // Get the signal generation resource name
  char *resName = calloc(PICO_MAX_RESOURCE_NAME_SIZE, 1);
  status = pico_getResourceName(ctx->system, *outRes, resName);
  if (status != 0) {
    free(resName);
    return pika_err(ctx, "pico_getResourceName", status);
  }

  status = pico_addResourceToVoiceDefinition(
      ctx->system, (pico_Char *)ctx->voiceName, (pico_Char *)resName);
  if (status != 0) {
    free(resName);
    return pika_err(ctx, "pico_addResourceToVoiceDefinition", status);
  }

  free(resName);
  return pika_OK;
}

/**
 * pika_validateOptions ensures opts is initialized correctly
 * @param opts: the pika options
 */
pika_Error pika_validateOptions(pika_Options opts) {
  if (strlen(opts.taFn) == 0) {
    return pika_err(NULL, "pika_validateOptions: taFn is empty",
                    PICO_ERR_INVALID_ARGUMENT);
  }
  if (strlen(opts.sgFn) == 0) {
    return pika_err(NULL, "pika_validateOptions: taFn is empty",
                    PICO_ERR_INVALID_ARGUMENT);
  }
  return pika_OK;
}

/**
 * pika_finiContext releases all resources associated with, and frees ctx
 * @param ctx: the pika context
 */
void pika_finiContext(pika_Context *ctx) {

  if (ctx->engine != NULL) {
    pico_disposeEngine(ctx->system, &ctx->engine);
    ctx->engine = NULL;
  }

  if (ctx->voiceName != NULL) {
    pico_releaseVoiceDefinition(ctx->system, (pico_Char *)ctx->voiceName);
    ctx->voiceName = NULL;
  }

  if (ctx->sgResource != NULL) {
    pico_unloadResource(ctx->system, &ctx->sgResource);
    ctx->sgResource = NULL;
  }

  if (ctx->taResource != NULL) {
    pico_unloadResource(ctx->system, &ctx->taResource);
    ctx->taResource = NULL;
  }

  pico_terminate(&ctx->system);
  ctx->system = NULL;

  if (ctx->taResourceName != NULL) {
    free(ctx->taResourceName);
    ctx->taResourceName = NULL;
  }

  if (ctx->sgResourceName != NULL) {
    free(ctx->sgResourceName);
    ctx->sgResourceName = NULL;
  }

  if (ctx->picoMem != NULL) {
    free(ctx->picoMem);
    ctx->picoMem = NULL;
  }

  free(ctx);
}

/**
 * pika_initContext initializes ctx based on opts
 * @param opts: the pika options
 * @param ctx: the pre-allocated pika context
 */
pika_Error pika_initContext(const pika_Options opts, pika_Context *ctx) {
  ctx->picoMemSize = 2500000;
  ctx->picoMem = malloc(ctx->picoMemSize);
  ctx->voiceName = pika_newVoiceName("PikaVoice");
  ctx->flushByte[0] = 0;

  pika_Error err = pika_validateOptions(opts);
  if (err.status != 0) {
    return err;
  }

  int status = pico_initialize(ctx->picoMem, ctx->picoMemSize, &ctx->system);
  if (status != 0) {
    return pika_err(ctx, "pico_initialize", status);
  }

  status = pico_createVoiceDefinition(ctx->system, (pico_Char *)ctx->voiceName);
  if (status != 0) {
    return pika_err(ctx, "pico_createVoiceDefinition", status);
  }

  err = pika_loadResource(ctx, opts.taFn, &ctx->taResource);
  if (err.status != 0) {
    return err;
  }

  err = pika_loadResource(ctx, opts.sgFn, &ctx->sgResource);
  if (err.status != 0) {
    return err;
  }

  status =
      pico_newEngine(ctx->system, (pico_Char *)ctx->voiceName, &ctx->engine);
  if (status != 0) {
    return pika_err(ctx, "pico_newEngine", status);
  }

  return pika_OK;
}

/**
 * pika_initContext frees ctx and releases all associated resources
 * @param ctx: the pika context
 */
void pika_fini(pika_Context *ctx) { pika_finiContext(ctx); }

/**
 * pika_init creates a new initialized pika context
 * @param opts: the pika options
 * @param ctx: pointer to store the context, if initialization succeeds
 */
pika_Error pika_init(const pika_Options opts, pika_Context **outCtx) {
  pika_Context *ctx = malloc(sizeof(pika_Context));

  pika_Error err = pika_initContext(opts, ctx);
  if (err.status != 0) {
    pika_finiContext(ctx);
    return err;
  }

  *outCtx = ctx;
  return pika_OK;
}

/**
 * pika_processData appends pending wav audio data to outWav
 * @param ctx: the pika context
 * @param outWav: storage for the wav data
 */
pika_Error pika_processData(pika_Context *ctx, pika_Bytes *outWav) {
  char buf[1 << 10];
  while (1) {
    pico_Int16 typ = 0;
    pico_Int16 len = 0;
    int status = pico_getData(ctx->engine, buf, 1 << 10, &len, &typ);
    *outWav = pika_appendBytes(*outWav, buf, len);
    switch (status) {
    case PICO_STEP_BUSY:
      continue;
    case PICO_STEP_IDLE:
      return pika_OK;
    default:
      return pika_err(ctx, "pika_processData: pico_getData", status);
    }
  }
}

/**
 * pika_flush flushes any remaining audio and appends it to outWav
 * @param ctx: the pika context
 * @param outWav: storage for the wav data
 */
pika_Error pika_flush(pika_Context *ctx, pika_Bytes *outWav) {
  pico_Int16 sent;
  int status =
      pico_putTextUtf8(ctx->engine, (pico_Char *)ctx->flushByte, 1, &sent);
  if (status != 0) {
    return pika_err(ctx, "pika_flush: pico_putTextUtf8", status);
  }
  return pika_processData(ctx, outWav);
}

/**
 * pika_synthesize converts text to 16-bit/mono/16khz wav audio
 *                 - if outWav is NULL or empty, a new object is constructed
 *                 - if outWav is not NULL, but is empty, the wav file header is
 * added
 * @param ctx: the pika context
 * @param text: the utf8 text to convert to audio
 * @param outWav: storage for the wav data
 */
pika_Error pika_synthesize(pika_Context *ctx, const char *text,
                           pika_Bytes *outWav) {

  if (outWav == NULL) {
    *outWav = pika_makeWavHeader();
  } else if (outWav->len == 0) {
    if (outWav->buf != NULL) {
      free(outWav->buf);
    }
    *outWav = pika_makeWavHeader();
  }

  pico_Int16 remaining = (pico_Int16)strlen(text);
  pico_Char *window = (pico_Char *)text;
  pico_Int16 sent = 0;
  while (remaining > 0) {
    int status = pico_putTextUtf8(ctx->engine, window, remaining, &sent);
    if (status != 0) {
      return pika_err(ctx, "pika_synthesize: pico_putTextUtf8: text", status);
    }

    remaining -= sent;
    window += sent;

    pika_Error err = pika_processData(ctx, outWav);
    if (err.status != 0) {
      return err;
    }
  }

  pika_Error err = pika_flush(ctx, outWav);
  if (err.status != 0) {
    return err;
  }
  pika_finalizeWavHeader(*outWav);
  return pika_OK;
}
