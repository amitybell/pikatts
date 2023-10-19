package pikatts

import (
	"testing"
)

func TestNew(t *testing.T) {
	eng, err := New(British)
	if err != nil {
		t.Fatal(err)
	}
	defer eng.Close()
}

func TestSynthesize(t *testing.T) {
	synth := func(v Voice) ([]byte, error) {
		eng, err := New(v)
		if err != nil {
			return nil, err
		}
		defer eng.Close()

		return eng.Synthesize("hello world")
	}

	voices := []Voice{
		American,
		British,
		French,
		German,
		Italian,
		Spanish,
	}

	for _, v := range voices {
		wav, err := synth(v)
		if err != nil {
			t.Fatal(err)
		}

		// wav header is 44 bytes, so if it's not more than that
		// then no valid data was generated
		if len(wav) <= 44 {
			t.Fatalf("Invalid output len %d", len(wav))
		}
	}
}
