package main

import (
	"fmt"
	"io/ioutil"
	"log"

	"github.com/shifr/goquant"
)

func main() {
	log.Println("Opening file")
	b, err := ioutil.ReadFile("/tmp/goo.png") // just pass the file
	if err != nil {
		fmt.Print(err)
	}
	log.Println("bytes,", len(b))

	log.Println("After bytes")

	goquant.Quantize(&b)

	err = ioutil.WriteFile("/tmp/ttt.png", b, 0777)
	if err != nil {
		fmt.Print(err)
	}
}
