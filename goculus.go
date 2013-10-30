package main

// #cgo LDFLAGS: -L. -lvr -lovr -ludev -lpthread -lX11 -lXinerama
// #include "vr_wrap.h"
import "C"

import (
	/* "ovr" */

	"errors"
	"fmt"
	"io/ioutil"

	"github.com/go-gl/gl"
	glfw "github.com/go-gl/glfw3"
)

func glfwErrorCB(err glfw.ErrorCode, desc string) {
	fmt.Printf("%v: %v\n", err, desc)
}

func findRiftDisplay(monitors []*glfw.Monitor) (*glfw.Monitor, error) {
	for _, m := range monitors {
		w, h := m.GetPhysicalSize()
		if w == 150 && h == 94 {
			return m, nil
		}
	}
	return nil, errors.New("Could not find rift")
}

func shaderFromSource(path string, shaderType gl.GLenum) gl.Shader {
	s := gl.CreateShader(shaderType)
	src, _ := ioutil.ReadFile("glsl/perspective.v.glsl")
	s.Source(string(src))
	s.Compile()
	fmt.Println(s.GetInfoLog())
	return s
}

func programFromShaders(shaders []gl.Shader) gl.Program {
	program := gl.CreateProgram()
	for _, s := range shaders {
		program.AttachShader(s)
	}
	program.Link()
	program.Validate()
	return program
}

func main() {
	glfw.SetErrorCallback(glfwErrorCB)

	if !glfw.Init() {
		panic("Couldn't init GLFW.")
	}
	defer glfw.Terminate()

	monitors, err := glfw.GetMonitors()
	if err != nil {
		panic("Couldn't get the list of monitors.")
	}
	rift, err := findRiftDisplay(monitors)

	glfw.WindowHint(glfw.Decorated, glfw.False)
	window, err := glfw.CreateWindow(1280, 800, "Goculus demo", nil, nil)
	if err != nil {
		panic("Couldn't create window.")
	}

	window.MakeContextCurrent()

	x, y := rift.GetPosition()
	window.SetPosition(x, y)

	glfw.SwapInterval(1)
	gl.Init()

	C.vr_init()
	defer C.vr_finish()

	gl.ClearColor(0.0, 0.0, 1.0, 1.0)
	vs := shaderFromSource("glsl/perspective.v.glsl", gl.VERTEX_SHADER)
	defer vs.Delete()
	fs := shaderFromSource("glsl/material.f.glsl", gl.FRAGMENT_SHADER)
	defer fs.Delete()
	shaders := [...]gl.Shader{vs, fs}
	program := programFromShaders(shaders[:])
	program.Use()
	defer program.Delete()
	fmt.Println(program.GetInfoLog())
	defer program.Delete()

	for !window.ShouldClose() {
		gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
		window.SwapBuffers()
		glfw.PollEvents()
	}
}
