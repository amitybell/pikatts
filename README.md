# About

Pika TTS is a small, local text to speech voice synthesizer package based on the [SVox Pico library included in Android AOSP](https://android.googlesource.com/platform/external/svox)

# Sample

A sample is available in [testdata/sample.wav](testdata/sample.wav)

# Usage

```go

import (
	"github.com/silenttwin/pikatts"
	"log"
)

func main() {
	eng, err := pikatts.New(pikatts.British)
	if err != nil {
		log.Fatalln("Failed to create TTS engine:", err)
	}
	defer eng.Close()

	wav, err := eng.Synthesize("hello world")
	if err != nil {
		log.Fatalln("Failed synthesize text:", err)
	}

	// Do something with `wav`
	_ = wav
}

```


# See Also

This package deals solely with text-to-speech synthesizing. Other packages can be used to further process or play the generated audio.

- https://github.com/gopxl/beep
- https://github.com/go-audio/audio
