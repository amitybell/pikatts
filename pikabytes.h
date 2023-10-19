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

#include <stdlib.h>

typedef struct {
  char *buf;
  int len;
  int cap;
} pika_Bytes;

pika_Bytes pika_append(pika_Bytes p, const pika_Bytes q);

pika_Bytes pika_appendBytes(pika_Bytes dst, const char *v, int len);

pika_Bytes pika_appendString(pika_Bytes dst, const char *v);

pika_Bytes pika_appendLE16(pika_Bytes dst, unsigned short v);

pika_Bytes pika_appendLE32(pika_Bytes dst, unsigned long v);

pika_Bytes pika_makeBytes(int cap);

void pika_putLE32(char *dst, unsigned long v);
