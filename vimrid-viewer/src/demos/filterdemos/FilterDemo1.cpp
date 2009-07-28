/* First attempt at image processing with the Sobel image filter.
 *
 * This implements a simple Sobel filter, convolutes the image in
 * two directions (X and Y), combines the result and then outputs
 * a truncated gradient (not normalised, yet) and then binds the
 * filter response as an OpenGL texture to some simple geometry.
 *
 * The second attempt (FilterDemo2) will rotate the Sobel filter
 * in 4 degrees, so that the following edges are found:
 *
 * - north and south facing X edges
 * - east and west facing Y edges
 *
 * FilterDemo2 will also attempt to normalise the output, between
 * a min and max of 0 and 255 so that truncation is not required,
 * which may well give a better result.
 *
 * In this class (FilterDemo1), as a substitute for normalisation,
 * the pixels are divided by 2 as they are combined which may not
 * be the best approach (instead normalisation will be used).
 */

#include "FilterDemo1.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <cmath>
#include <map>

#define TEMP_PORT 1234
#define TEMP_HOST "localhost"
#define BUFFER_SIZE 256

using namespace std;
using namespace vimrid::imaging;
using namespace vimrid::dicom;
using namespace vimrid::input;

namespace vimrid
{
namespace demos
{
namespace filterdemos
{

FilterDemo1::FilterDemo1(VimridSettings &settings)
	: GlutApplication(settings)
{
	Title = "Filter demo 1 - Tue 10 Mar";
	EnableCursor = true;
	CameraTrim.Z = -4;
}

FilterDemo1::~FilterDemo1()
{
}

void FilterDemo1::Init()
{
	this->GlutApplication::Init();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(2, textureIds);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Load the DICOM image from the server.
	WRITE_DEBUG_TEXT("Loading DICOM image from server...");
	DicomImage &dicomImage = *loadDicomImage(GetSettings().Input, TEMP_HOST, TEMP_PORT);

	// Copy the DICOM image pixels to a new image matrix.
	WRITE_DEBUG_TEXT("Copying DICOM image to new image matrix...");
	ImageMatrix &sourceMatrix = *dicomImage.ToImageMatrix();

	// Change the bit length to 8-bit to work with OpenGL.
	WRITE_DEBUG_TEXT("Converting original to 8-bit image...");
	dicomImage.ChangeBitLength(V_BYTE_LENGTH);
	createTexture(&textureIds[0], dicomImage);

	ImageMatrix sobelX(3, 3, 1);
	/*sobelX[0][0][0] = 1;
	sobelX[0][1][0] = 2;
	sobelX[0][2][0] = 1;
	sobelX[1][0][0] = 0;
	sobelX[1][1][0] = 0;
	sobelX[1][2][0] = 0;
	sobelX[2][0][0] = -1;
	sobelX[2][1][0] = -2;
	sobelX[2][2][0] = -1;*/
	VInt32 sobelXPixels[3][3] =
	{
		{+1, +2, +1},
		{0, 0, 0},
		{-1, -2, -1}
	};
	sobelX.LoadPixels(*sobelXPixels);

	ImageMatrix sobelY(3, 3, 1);
	/*sobelY[0][0][0] = -1;
	sobelY[0][1][0] = 0;
	sobelY[0][2][0] = 1;
	sobelY[1][0][0] = -2;
	sobelY[1][1][0] = 0;
	sobelY[1][2][0] = 2;
	sobelY[2][0][0] = -1;
	sobelY[2][1][0] = 0;
	sobelY[2][2][0] = 1;*/
	VInt32 sobelYPixels[3][3] =
	{
		{-1, 0, +1},
		{-2, 0, +2},
		{-1, 0, +1}
	};
	sobelY.LoadPixels(*sobelYPixels);

	WRITE_DEBUG_TEXT("Convoluting with Sobel X...");
	ImageMatrix &sobelResultX = *convoluteImage(sourceMatrix, sobelX);

	WRITE_DEBUG_TEXT("Convoluting with Sobel Y...");
	ImageMatrix &sobelResultY = *convoluteImage(sourceMatrix, sobelY);

	WRITE_DEBUG_TEXT("Combining filter responses...");
	ImageMatrix &combinedResult = *combineImages(sobelResultX, sobelResultY);

	createTexture(&textureIds[1], combinedResult);

	delete &sobelResultX;
	delete &sobelResultY;
	delete &combinedResult;
	delete &sourceMatrix;
	delete &dicomImage;
}

ImageMatrix *FilterDemo1::combineImages(const ImageMatrix &image1, const ImageMatrix &image2)
{
	if (image1.GetRowCount() != image2.GetRowCount())
	{
		THROW_VIMRID_EX(
			"The first matrix row count (" << image1.GetRowCount() <<
			") does not match that of the second (" << image2.GetRowCount() << ").");
	}

	if (image1.GetColumnCount() != image2.GetColumnCount())
	{
		THROW_VIMRID_EX(
			"The first matrix column count (" << image1.GetColumnCount() <<
			") does not match that of the second (" << image2.GetColumnCount() << ").");
	}

	if (image1.GetChannelCount() != image2.GetChannelCount())
	{
		THROW_VIMRID_EX(
			"The first matrix channel count (" << image1.GetChannelCount() <<
			") does not match that of the second (" << image2.GetChannelCount() << ").");
	}

	// Create empty result the same size as source.
	ImageMatrix *result = new ImageMatrix(
		image1.GetRowCount(),
		image1.GetColumnCount(),
		image1.GetChannelCount());

	VUInt32 srIndex; // Source row index
	VUInt32 scIndex; // Source column index

	VUInt32 srCount = image1.GetRowCount(); // Source row count
	VUInt32 scCount = image1.GetColumnCount(); // Source columns count

	for (srIndex = 0; srIndex < srCount; srIndex++)
	{
		for (scIndex = 0; scIndex < scCount; scIndex++)
		{
			/*VInt32 pixel1 = image1[srIndex][scIndex][0];
			VInt32 pixel2 = image2[srIndex][scIndex][0];

			// TODO: Normalise instead of just dividing by 2.
			result[srIndex][scIndex][0] = (VInt32)((VDouble)(pixel1 + pixel2) / 2);*/

			VInt32 pixel1 = image1.Get(srIndex, scIndex, 0);
			VInt32 pixel2 = image2.Get(srIndex, scIndex, 0);

			// TODO: Normalise instead of just dividing by 2.
			result->Get(srIndex, scIndex, 0) = (VInt32)((VDouble)(pixel1 + pixel2) / 2);
		}
	}

	return result;
}

/* Reverse engineered from Filter.java which was
 * written by Wenda He (Aberystwyth University).
 *
 * TODO: Move to separate class.
 */
ImageMatrix *FilterDemo1::convoluteImage(const ImageMatrix &source, const ImageMatrix &filter)
{
	// Create empty result the same size as source.
	ImageMatrix *result = new ImageMatrix(
		source.GetRowCount(),
		source.GetColumnCount(),
		source.GetChannelCount());

	VUInt32 srIndex; // Source row index
	VUInt32 scIndex; // Source column index
	VInt32 frIndex; // Filter row index
	VInt32 fcIndex; // Filter column index

	VUInt32 srCount = source.GetRowCount(); // Source row count
	VUInt32 scCount = source.GetColumnCount(); // Source columns count
	VUInt32 frCount = filter.GetRowCount(); // Filter row count
	VUInt32 fcCount = filter.GetColumnCount(); // Filter column count

	// Run the filter over each pixel in the image.
	for (srIndex = 0; srIndex < srCount; srIndex++)
	{
		for (scIndex = 0; scIndex < scCount; scIndex++)
		{
			/* The convoluted pixel to be stored; the result of each
			 * matrix transformation is accumulated on each value of the
			 * filter matrix.
			 */
			VInt32 filterResponsePixel = 0;

			/* TODO: Explain why are we starting at minus values...
			 *
			 * Correct:?
			 * Don't start the filter at the very top left, but start inward
			 * at half of the filter size so that we don't touch the boundary
			 * of the image (as this wouldn't look very nice).
			 */
			VInt32 frStart = (VInt32)-((VFloat)frCount / 2);
			VInt32 fcStart = (VInt32)-((VFloat)fcCount / 2);
			VInt32 frEnd = (VInt32)((VFloat)frCount / 2);
			VInt32 fcEnd = (VInt32)((VFloat)fcCount / 2);

			/* For each value in the filter matrix, apply convolution
			 * and accumulate the result for each pixel.
			 */
			for (frIndex = frStart; frIndex <= frEnd; frIndex++)
			{
				for (fcIndex = fcStart; fcIndex <= fcEnd; fcIndex++)
				{
					/* Calculate the indices to be used for accessing the
					 * source image pixels.
					 *
					 * TODO: Explain the first 2 blocks of code. Why is so
					 * much adding and subtraction going on? Is there a simpler
					 * way of doing the same thing?
					 */

					VInt32 rowIndex = srIndex + frIndex;
					if (rowIndex < 0)
					{
						rowIndex += srCount;
					}
					else if (rowIndex >= (VInt32)srCount)
					{
						rowIndex -= srCount;
					}

					VInt32 columnIndex = scIndex + fcIndex;
					if (columnIndex < 0)
					{
						columnIndex += scCount;
					}
					else if (columnIndex >= (VInt32)scCount)
					{
						columnIndex -= scCount;
					}

					/* Calculate the indices to be used for accessing the filter value.
					 *
					 * TODO: Explain the arithmetic used; i.e. why is the index
					 * added to the total columns then divided by 2...
					 */
					VInt32 useFrIndex = frIndex + frCount / 2;
					VInt32 useFcIndex = fcIndex + fcCount / 2;

					/* Get the source image pixel and filter value to be applied;
					 * nothing complicated here, just declaring these as variables
					 * to make the code more readable.
					 */
					/*VDouble sourcePixel = (VDouble)source[rowIndex][columnIndex][0];
					VDouble filterValue = (VDouble)filter[useFrIndex][useFcIndex][0];*/
					VDouble sourcePixel = (VDouble)source.Get(rowIndex, columnIndex, 0);
					VDouble filterValue = (VDouble)filter.Get(useFrIndex, useFcIndex, 0);

					/* Multiply the pixel value with each value of the filter matrix;
					 * because we want the result of the entire matrix against each
					 * pixel, the value is accumulated and set to the result image.
					 */
					VDouble convolvingValue = sourcePixel * filterValue;
					filterResponsePixel += (VInt32)round(convolvingValue);
				}
			}

			// After the convolution on this particular pixel, set the result.
			//result[srIndex][scIndex][0] = filterResponsePixel;
			result->Get(srIndex, scIndex, 0) = filterResponsePixel;
		}
	}
	return result;
}

void FilterDemo1::createTexture(
	GLuint *textureId,
	const DicomImage &dicomImage)
{
	createTexture(
		textureId,
		dicomImage.GetColumnCount(),
		dicomImage.GetRowCount(),
		dicomImage.CopyPixelsToUByte());
}

void FilterDemo1::createTexture(
	GLuint *textureId,
	const ImageMatrix &image)
{
	createTexture(
		textureId,
		image.GetColumnCount(),
		image.GetRowCount(),
		image.CopyPixelsToUByte());
}

void FilterDemo1::createTexture(
	GLuint *textureId,
	GLuint width,
	GLuint height,
	const GLubyte *pixels)
{
	glBindTexture(GL_TEXTURE_2D, *textureId);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height,
		0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);

	delete[] pixels;
}

void FilterDemo1::Update()
{
	this->GlutApplication::Update();

	const MouseInfo &mouse = GetInputManager()->GetMouseInfo();

	// When mouse moves up and down, move toward/away from screen.
	CameraPoint.X = (((GetWindowSize().Width / 2) * -1) + mouse.GetPosition().X) * 0.15;
	CameraPoint.Y = (((GetWindowSize().Height / 2) * -1) + mouse.GetPosition().Y) * -0.2;

	if (IsSpecialKeyPressed(GLUT_KEY_UP))
	{
		CameraPoint.Z += .01;
	}

	if (IsSpecialKeyPressed(GLUT_KEY_DOWN))
	{
		CameraPoint.Z -= .01;
	}
}

void FilterDemo1::Render()
{
	this->GlutApplication::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	// Use model view so that rotation value is literal, not added.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(0, 0, CameraTrim.Z + CameraPoint.Z);
	glRotatef(CameraTrim.Y + CameraPoint.Y, 1, 0, 0);
	glRotatef(-(CameraTrim.X + CameraPoint.X), 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, textureIds[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-2.1, +1.0, 0.0); // top left
	glTexCoord2f(0.0, 1.0); glVertex3f(-2.1, -1.0, 0.0); // bottom left
	glTexCoord2f(1.0, 1.0); glVertex3f(-0.1, -1.0, 0.0); // bottom right
	glTexCoord2f(1.0, 0.0); glVertex3f(-0.1, +1.0, 0.0); // top right
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textureIds[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(+0.1, +1.0, 0.0); // top left
	glTexCoord2f(0.0, 1.0); glVertex3f(+0.1, -1.0, 0.0); // bottom left
	glTexCoord2f(1.0, 1.0); glVertex3f(+2.1, -1.0, 0.0); // bottom right
	glTexCoord2f(1.0, 0.0); glVertex3f(+2.1, +1.0, 0.0); // top right
	glEnd();

	glFlush();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	FinishRender();
}

void FilterDemo1::Reshape()
{
	this->GlutApplication::Reshape();

	glViewport(0, 0, (GLsizei)GetWindowSize().Width, (GLsizei)GetWindowSize().Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)GetWindowSize().Width / (GLfloat)GetWindowSize().Height, 1.0, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// TODO: Move this code into a separate class, like DicomClient.
DicomImage *FilterDemo1::loadDicomImage(string fileName, string hostName, int port)
{
	// Buffer for read and write.
    char buffer[BUFFER_SIZE];

    WRITE_DEBUG_TEXT("Connecting to " << hostName << ":" << port << "...");

    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0)
    {
        THROW_VIMRID_EX("Could not open socket.");
    }

    struct hostent *server;
    server = gethostbyname(hostName.c_str());
    if (server == NULL)
    {
    	THROW_VIMRID_EX("Host '" << hostName << "' does not exist.");
    }

    struct sockaddr_in serverAddress;
    bzero((char*)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, server->h_length);

    serverAddress.sin_port = htons(port);
    int connectResult = connect(
		socketFD,
		(const sockaddr*)&serverAddress,
		sizeof(serverAddress));

    if (connectResult < 0)
    {
    	THROW_VIMRID_EX("Could not connect to host ("
    		<< TEMP_HOST << ":" << TEMP_PORT << ").");
    }

    WRITE_DEBUG_TEXT("Requesting file name " << fileName << "...");

    int writeResult = write(socketFD, fileName.c_str(), fileName.length());
    if (writeResult < 0)
    {
    	THROW_VIMRID_EX("Could not write to socket for DICOM file name.");
    }

    WRITE_DEBUG_TEXT("Downloading DICOM data from server...");

    bool readData = true;
    string readString;

    /* TODO: Consider reading an int array rather than deserialisng
     * a string which takes up a lot of overhead.
     */
    while (readData)
	{
		// Read a bit at a time, eventually "end" string will be received.
		bzero(buffer, BUFFER_SIZE);
		int readResult = read(socketFD, buffer, BUFFER_SIZE - 1);
		if (readResult < 0)
		{
			THROW_VIMRID_EX("Could not read from socket.");
		}

		// Append the received data to the existing data.
		readString.append(buffer);

		// Continue reading while end is not found.
		readData = readString.find(DicomImage::EndFile) == string::npos;

		// If an error occurs, break the loop and quit.
	    if (readString.find(DicomImage::Error) != string::npos)
	    {
	    	THROW_VIMRID_EX("Cannot get DICOM data (server error).");
	    }
	}

    // Create image object which deserialises the pixels.
    DicomImage *image = new DicomImage(readString);

    int closeResult = close(socketFD);
    if (closeResult < 0)
    {
		THROW_VIMRID_EX("Could not close connection to server.");
    }

    WRITE_DEBUG_TEXT("DICOM image downloaded successfully.");

    return image;
}

}
}
}
