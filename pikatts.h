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

#include "picoapi.h"
#include "picoapid.h"
#include "picoos.h"
#include "pikawav.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  pico_System system;
  const char *message;
  int status;
} pika_Error;

extern const pika_Error pika_OK;

typedef struct {
  const char *taFn;
  const char *sgFn;
} pika_Options;

typedef struct {
  pico_System system;
  char *voiceName;
  pico_Resource taResource;
  char *taResourceName;
  pico_Resource sgResource;
  char *sgResourceName;
  pico_Engine engine;
  void *picoMem;
  int picoMemSize;
  char flushByte[1];
} pika_Context;

pika_Error pika_init(const pika_Options opts, pika_Context **outCtx);

void pika_fini(pika_Context *ctx);

pika_Error pika_synthesize(pika_Context *ctx, const char *text,
                           pika_Bytes *outWav);

char *pika_error_message(pika_Error err);
