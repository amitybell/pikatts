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

  // picoos_WriteStr(f, (picoos_char *)"RIFF");
  hdr = pika_appendString(hdr, "RIFF");
  // picoos_write_le_uint32(f, dataLength + 36);
  hdr = pika_appendLE32(hdr, dataLength + 36);
  // picoos_WriteStr(f, (picoos_char *)"WAVE");
  hdr = pika_appendString(hdr, "WAVE");
  // picoos_WriteStr(f, (picoos_char *)"fmt ");
  hdr = pika_appendString(hdr, "fmt ");
  // picoos_write_le_uint32(f, 16);
  hdr = pika_appendLE32(hdr, 16);
  // picoos_write_le_uint16(f, formatTag);
  hdr = pika_appendLE16(hdr, formatTag);
  // picoos_write_le_uint16(f, 1);
  hdr = pika_appendLE16(hdr, 1);
  // picoos_write_le_uint32(f, sampleRate);
  hdr = pika_appendLE32(hdr, sampleRate);
  // picoos_write_le_uint32(f, bytesPerSec);
  hdr = pika_appendLE32(hdr, bytesPerSec);
  // picoos_write_le_uint16(f, blockAlign);
  hdr = pika_appendLE16(hdr, blockAlign);
  // picoos_write_le_uint16(f, sampleSize);
  hdr = pika_appendLE16(hdr, sampleSize);
  // picoos_WriteStr(f, (picoos_char *)"data");
  hdr = pika_appendString(hdr, "data");
  // picoos_write_le_uint32(f, dataLength);
  hdr = pika_appendLE32(hdr, dataLength);
  // (*hdrSize) = 44;
  return hdr;
}
