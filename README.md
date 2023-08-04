
<h1 align="center">Digital Image Processing 👁️‍🗨️🖥️</h1>

<p align="center">
  <a href="#features">Features</a> •
  <a href="#gt">Getting started</a> • 
  <a href="#if">Input Format</a> • 
  <a href="#of">Output Format</a> • 
  <a href="#ex">Examples</a> • 
  <a href="#ak">Aknowledgements</a>
</p>

<p align="center">
This repository contains a C program designed for performing basic image operations on PGM (Portable Gray Map) images. The program enables reading PGM image files, applying operations such as logarithm transformation, contrast stretching, region filling (similar to the "paint bucket" tool in Paint applications), and saving the resulting image to a new file.
</p>


## <div id="features"></div>Features
<p align="justify">
The program includes functionalities for point operations (logarithm, contrast stretching, etc.) and digital filters (mean filter, median filter, Gaussian smoothing). It also provides a prototype histogram generation feature. Additionally, there are functions for reading and saving PGM images, and handling image memory allocation and deallocation.
</p>  

Full explanation of each operation can be found here: https://homepages.inf.ed.ac.uk/rbf/HIPR2/wksheets.htm

## <div id="gt"></div>Getting started

- Clone the repository to your local machine
```
1. git clone https://github.com/MatheusPaivaa/digital-processing-project
2. cd imageGenerator
```
- Compile the program using a C compiler (e.g., GCC).
```
3. gcc -std=c99 .\image_generator.c -o imgGen 
```
- Run the compiled executable.
```
4. .\imgGen.exe
```
- Provide the name of a PGM image file.
```
5. PGM image file: exampleImage.pgm
```
- Choose the desired operation or filter.
- The resulting image will be saved as "imagemGerada.pgm".

**Note:** The code's comments provide detailed explanations of functions, operations, and logic.

## <div id="if"></div>Input Format

- Provide the name of the PGM image file (e.g., image.pgm) when prompted.
- Choose an operation category and select a specific operation.
- For certain operations (e.g., thresholding), additional input may be required (e.g., threshold value).
  
**Note:** The program is compatible with PGM images, catering to both ASCII (P2) and binary (P5) formats. It effortlessly identifies the format and reads the image accordingly.

## <div id="of"></div>Output Format

- Processed images are saved as PGM files, named "imagemGerada.pgm".
- The modified image will be generated in the P2 (ASCII) format, regardless of the input format.
- For histogram operation, the histogram values are printed, and a basic graphical representation of the histogram is displayed.

## <div id="ex"></div>Examples

## <div id="ak"></div>Aknowledgements
<p align="justify">
Please note that this program is designed for educational purposes and demonstrates image processing techniques in C. This is an extended version of the final project from the discipline Introduction do Computer Science (SCC-221) at the Institute of Mathematics and Computer Science (University of São Paulo).
</p>

## License
This project could be used by anyone! MIT License.
