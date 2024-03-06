package main

import (
	"encoding/csv"
	"image"
	"image/color"
	"image/png"
	"log"
	"os"
	"path/filepath"
	"strconv"
)

func main() {
	// Path to the CSV file
	userProfile := os.Getenv("USERPROFILE")
	filePath := filepath.Join(userProfile, "AlpacaBot", "Collision Data", "collision_data.csv")

	// Open the CSV file
	file, err := os.Open(filePath)
	if err != nil {
		log.Fatalf("Failed to open file: %v", err)
	}
	defer file.Close()

	// Read the CSV file
	reader := csv.NewReader(file)
	records, err := reader.ReadAll()
	if err != nil {
		log.Fatalf("Failed to read file: %v", err)
	}

	// Group records by plane
	dataByPlane := make(map[string][][]string)
	for _, record := range records[1:] { // Skip the header row
		plane := record[2]
		dataByPlane[plane] = append(dataByPlane[plane], record)
	}

	for plane, records := range dataByPlane {
		// Find the min and max x, y coordinates
		minX, minY, maxX, maxY := 1<<31-1, 1<<31-1, 0, 0
		for _, record := range records {
			x, err := strconv.Atoi(record[0])
			if err != nil {
				continue
			}
			y, err := strconv.Atoi(record[1])
			if err != nil {
				continue
			}
			if x < minX {
				minX = x
			}
			if y < minY {
				minY = y
			}
			if x > maxX {
				maxX = x
			}
			if y > maxY {
				maxY = y
			}
		}

		// Create an empty image
		width, height := maxX-minX+1, maxY-minY+1
		img := image.NewRGBA(image.Rect(0, 0, width, height))

		// Fill in the image with data
		for _, record := range records {
			x, err := strconv.Atoi(record[0])
			if err != nil {
				continue
			}
			y, err := strconv.Atoi(record[1])
			if err != nil {
				continue
			}
			flag, err := strconv.Atoi(record[3])
			if err != nil {
				continue
			}

			// Create a base color (dark gray)
			r, g, b := 64, 64, 64

			// Adjust the color based on the flag value
			if flag&0xFFFFFF != 0 {
				r, g, b = 128, 0, 0 // CLOSED: Dark red
			}
			if flag&0x1000000 != 0 {
				r, g, b = 0, 0, 128 // UNINITIALIZED: Dark blue
			}
			if flag&0x100 != 0 {
				r, g, b = 128, 128, 0 // OCCUPIED: Dark yellow
			}
			if flag&0x20000 != 0 {
				r, g, b = 75, 0, 130 // SOLID: Dark purple (indigo)
			}
			if flag&0x200000 != 0 {
				r, g, b = 128, 64, 0 // BLOCKED: Dark orange
			}

			// Set the pixel in the image
			col := color.RGBA{uint8(r), uint8(g), uint8(b), 255}
			img.Set(x-minX, height-(y-minY)-1, col)
		}

		// Save the image to a file
		outputFileName := "collision_map_plane_" + plane + ".png"
		outputFile, err := os.Create(outputFileName)
		if err != nil {
			log.Fatalf("Failed to create output file: %v", err)
		}
		defer outputFile.Close()

		png.Encode(outputFile, img)
	}
}
