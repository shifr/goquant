package goquant

/*
#cgo CFLAGS: -I ${SRCDIR}/src
#cgo CFLAGS: -I ${SRCDIR}/src/lib
#cgo LDFLAGS: -L /usr/local/lib -limagequant -lrwpng
#include "goquant.h"
*/
import "C"
import (
	"log"
	"unsafe"
)

func Quantize(img_buff *[]byte) {
	in_image_size := len(*img_buff)
	out_image_size := C.size_t(0)

	log.Println("IN Image size: ", in_image_size)
	b := *img_buff

	bu := C.custom_create(unsafe.Pointer(&b[0]), C.int(in_image_size), &out_image_size)
	log.Println("OUT Image size: ", out_image_size)

	log.Println("Fetch out image from C to Go bytes")
	*img_buff = C.GoBytes(unsafe.Pointer(bu), C.int(out_image_size))

	log.Println("Free C object")
	C.free(unsafe.Pointer(bu))
}
