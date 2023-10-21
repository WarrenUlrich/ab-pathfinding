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
		plane := record[1]
		dataByPlane[plane] = append(dataByPlane[plane], record)
	}

	for plane, records := range dataByPlane {
		// Find the min and max x, y coordinates
		minX, minY, maxX, maxY := 1<<31-1, 1<<31-1, 0, 0
		for _, record := range records {
			x, err := strconv.Atoi(record[2])
			if err != nil {
				continue
			}
			y, err := strconv.Atoi(record[3])
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
			x, err := strconv.Atoi(record[2])
			if err != nil {
				continue
			}
			y, err := strconv.Atoi(record[3])
			if err != nil {
				continue
			}
			flag, err := strconv.Atoi(record[4])
			if err != nil {
				continue
			}

			// Determine the color based on the flag value
			var col color.Color
			switch flag {
			case 0:
				col = color.RGBA{0, 255, 0, 255}
			case 0xFFFFFF:
				col = color.RGBA{255, 0, 0, 255}
			case 0x1000000:
				col = color.RGBA{0, 0, 255, 255}
			case 0x100:
				col = color.RGBA{255, 255, 0, 255}
			case 0x20000:
				col = color.RGBA{128, 0, 128, 255}
			case 0x200000:
				col = color.RGBA{255, 128, 0, 255}
			case 0x2:
				col = color.RGBA{0, 255, 255, 255}
			case 0x8:
				col = color.RGBA{255, 0, 255, 255}
			case 0x20:
				col = color.RGBA{128, 128, 0, 255}
			case 0x80:
				col = color.RGBA{128, 128, 128, 255}
			case 0x4:
				col = color.RGBA{0, 128, 0, 255}
			case 0x10:
				col = color.RGBA{128, 0, 0, 255}
			case 0x40:
				col = color.RGBA{0, 0, 128, 255}
			case 0x1:
				col = color.RGBA{0, 128, 128, 255}
			default:
				col = color.RGBA{0, 0, 0, 255}
			}

			// Set the pixel in the image
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
