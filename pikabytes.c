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

#include "pikabytes.h"
#include <stddef.h>
#include <string.h>

pika_Bytes pika_append(pika_Bytes p, const pika_Bytes q) {
  if (q.len == 0) {
    return p;
  }

  int len = p.len + q.len;
  if (len + 1 < p.cap) {
    memcpy(p.buf + p.len, q.buf, q.len);
    p.len += q.len;
    p.buf[p.len] = 0;
    return p;
  }

  int cap = (len * 2) + 1;
  pika_Bytes r = {
      .buf = malloc(cap),
      .len = len,
      .cap = cap,
  };
  memcpy(r.buf, p.buf, p.len);
  memcpy(r.buf + p.len, q.buf, q.len);
  r.buf[r.len] = 0;
  free(p.buf);
  return r;
}

pika_Bytes pika_appendBytes(pika_Bytes dst, const char *v, int len) {
  // we never modify the src buffer, so it's safe to cast away const
  pika_Bytes b = {.buf = (char *)v, .len = len, .cap = len};
  return pika_append(dst, b);
}

pika_Bytes pika_appendString(pika_Bytes dst, const char *v) {
  int len = strlen(v);
  // we never modify the src buffer, so it's safe to cast away const
  pika_Bytes b = {.buf = (char *)v, .len = len, .cap = len};
  return pika_append(dst, b);
}

pika_Bytes pika_appendLE16(pika_Bytes dst, unsigned short v) {
  char b[2];
  b[0] = ((v)&0x00FF);
  b[1] = (((v)&0xFF00) >> 8);
  pika_Bytes src = {.buf = b, .len = 2};
  return pika_append(dst, src);
}

pika_Bytes pika_appendLE32(pika_Bytes dst, unsigned long v) {
  char b[4];
  b[0] = (v & 0x000000FF);
  b[1] = ((v & 0x0000FF00) >> 8);
  b[2] = ((v & 0x00FF0000) >> 16);
  b[3] = ((v & 0xFF000000) >> 24);
  pika_Bytes src = {.buf = b, .len = 4};
  return pika_append(dst, src);
}

pika_Bytes pika_makeBytes(int cap) {
  cap += 1; // + null byte
  pika_Bytes b = {.buf = malloc(cap), .len = 0, .cap = cap};
  return b;
}
