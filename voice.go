package pikatts

import (
	"github.com/silenttwin/pikatts/internal/lingware"
)

// Voice hold details about the text-to-speech voice files
type Voice struct {
	// TaData is the file content of the corresponding ..._ta.bin file
	TaData []byte
	// SgData is the file content of the corresponding ..._sg.bin file
	SgData []byte
}

var (
	// American holds voice data for American English
	American = mustCreateVoice("en-US_ta.bin", "en-US_lh0_sg.bin")
	// British holds voice data for British English
	British = mustCreateVoice("en-GB_ta.bin", "en-GB_kh0_sg.bin")
	// French holds voice data for French
	French = mustCreateVoice("fr-FR_ta.bin", "fr-FR_nk0_sg.bin")
	// German holds voice data for German
	German = mustCreateVoice("de-DE_ta.bin", "de-DE_gl0_sg.bin")
	// Italian holds voice data for Italian
	Italian = mustCreateVoice("it-IT_ta.bin", "it-IT_cm0_sg.bin")
	// Spanish holds voice data for Spanish
	Spanish = mustCreateVoice("es-ES_ta.bin", "es-ES_zl0_sg.bin")
)

// mustReadVoiceData reads the named embedded lingware file or panics
func mustReadVoiceData(name string) []byte {
	s, err := lingware.Files.ReadFile(name)
	if err != nil {
		panic(err)
	}
	if len(s) == 0 {
		panic("len(" + name + ") == 0")
	}
	return s
}

// mustCreateVoice creates an initialized Voice object
func mustCreateVoice(
	taFn string,
	sgFn string,
) Voice {
	return Voice{
		TaData: mustReadVoiceData(taFn),
		SgData: mustReadVoiceData(sgFn),
	}
}
