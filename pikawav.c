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

#include "pikawav.h"
#include "picoos.h"

pika_Bytes pika_makeWavHeader() {
  picoos_uint16 formatTag = FORMAT_TAG_LIN;
  picoos_uint32 sampleRate = SAMPLE_FREQ_16KHZ;
  picoos_uint32 bytesPerSample = 2;
  picoos_uint32 bytesPerSec = (sampleRate * bytesPerSample);
  picoos_uint16 blockAlign = bytesPerSample;
  picoos_uint16 sampleSize = 16;
  // use a dummy length for now
  picoos_uint32 dataLength = (bytesPerSample * 100000000);

  // real file sizes are usually at least a few KiB, so just allocate some extra
  pika_Bytes hdr = pika_makeBytes(4 << 10);

  hdr = pika_appendString(hdr, "RIFF");
  hdr = pika_appendLE32(hdr, dataLength + 36);
  hdr = pika_appendString(hdr, "WAVE");
  hdr = pika_appendString(hdr, "fmt ");
  hdr = pika_appendLE32(hdr, 16);
  hdr = pika_appendLE16(hdr, formatTag);
  hdr = pika_appendLE16(hdr, 1);
  hdr = pika_appendLE32(hdr, sampleRate);
  hdr = pika_appendLE32(hdr, bytesPerSec);
  hdr = pika_appendLE16(hdr, blockAlign);
  hdr = pika_appendLE16(hdr, sampleSize);
  hdr = pika_appendString(hdr, "data");
  hdr = pika_appendLE32(hdr, dataLength);
  return hdr;
}

void pika_finalizeWavHeader(pika_Bytes wav) {
  // file size section comes after "RIFF"
  pika_putLE32(4 + wav.buf, (unsigned long)wav.len);
  // data size section comes a the end of the 44 byte header
  pika_putLE32(wav.buf + 44 - 4, (unsigned long)wav.len - 44);
}
