package pikatts

/*
#cgo CFLAGS: -w
#cgo LDFLAGS: -lc -lm

#include "pikatts.h"
*/
import "C"

import (
	"errors"
	"fmt"
	"os"
	"path/filepath"
	"unsafe"
)

var (
	// ErrClosed is the error returned for operations on a closed Engine
	ErrClosed = errors.New("Closed")
)

// pikaErr converts e to a Go error
func pikaErr(e C.pika_Error) error {
	if e.status == 0 {
		return nil
	}
	msgC := C.pika_error_message(e)
	defer C.free((unsafe.Pointer)(msgC))
	return errors.New(C.GoString(msgC))
}

// Engine holds data for the text-to-speech synthesizer
type Engine struct {
	ctx *C.pika_Context
}

// Synthesize converts text to 16-bit/mono/16khz wav audio data
func (e *Engine) Synthesize(text string) (wav []byte, err error) {
	if e.ctx == nil {
		return nil, ErrClosed
	}

	var textC = C.CString(text)
	defer C.free((unsafe.Pointer)(textC))

	var wavC C.pika_Bytes
	defer func() { C.free((unsafe.Pointer)(wavC.buf)) }()

	err = pikaErr(C.pika_synthesize(e.ctx, textC, &wavC))
	if err != nil {
		return nil, err
	}

	return C.GoBytes((unsafe.Pointer)(wavC.buf), wavC.len), nil
}

// Close releases all resources associated with the synthesizer
func (e *Engine) Close() error {
	if e.ctx == nil {
		return ErrClosed
	}

	C.pika_fini(e.ctx)
	e.ctx = nil
	return nil
}

// New creates a new synthesizer for the specified voice
func New(voice Voice) (*Engine, error) {
	tmp, err := os.MkdirTemp("", "pikatts.")
	if err != nil {
		return nil, fmt.Errorf("setLang: create temp dir: %s", err)
	}
	defer os.RemoveAll(tmp)

	taFn := filepath.Join(tmp, "ta.bin")
	if err := os.WriteFile(taFn, voice.TaData, 0600); err != nil {
		return nil, fmt.Errorf("setLang: write ta.bin: %s", err)
	}
	taFnC := C.CString(taFn)
	defer C.free((unsafe.Pointer)(taFnC))

	sgFn := filepath.Join(tmp, "sg.bin")
	if err := os.WriteFile(sgFn, voice.SgData, 0600); err != nil {
		return nil, fmt.Errorf("setLang: write sg.bin: %s", err)
	}
	sgFnC := C.CString(sgFn)
	defer C.free((unsafe.Pointer)(sgFnC))

	eng := &Engine{}

	var opts C.pika_Options
	opts.taFn = taFnC
	opts.sgFn = sgFnC

	e := C.pika_init(opts, &eng.ctx)
	if err := pikaErr(e); err != nil {
		return nil, err
	}
	return eng, nil
}
