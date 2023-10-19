package lingware

import (
	"embed"
)

var (
	// Files holds the embedded bin files
	//
	//go:embed *.bin
	Files embed.FS
)
