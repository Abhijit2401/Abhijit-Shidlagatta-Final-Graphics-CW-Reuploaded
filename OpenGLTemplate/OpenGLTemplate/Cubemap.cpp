// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file modified.
// Major logic written/edited by me:
// - Fixed FreeImage height parsing bugs from original template
// ---------------------------------------------------------
#include "Common.h"
#include "Cubemap.h"
#include "include\freeimage\FreeImage.h"
#pragma comment(lib, "lib/FreeImage.lib")

bool CCubemap::LoadTexture(string filename, BYTE** bmpBytes, int& iWidth, int& iHeight)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(filename.c_str(), 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(filename.c_str());

	if (fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename.c_str());

	if (!dib) {
		char message[1024];
		sprintf_s(message, "Cannot load image\n%s\n", filename.c_str());
		MessageBox(NULL, message, "Error", MB_ICONERROR);
		return false;
	}

	iWidth = FreeImage_GetWidth(dib);
	// original code checked width twice so it is fixed it to check height properly
	iHeight = FreeImage_GetHeight(dib);
	int bpp = FreeImage_GetBPP(dib);

	BYTE* bDataPointer = FreeImage_GetBits(dib);

	if (bDataPointer == NULL || iWidth == 0 || iHeight == 0) {
		FreeImage_Unload(dib);
		return false;
	}

	*bmpBytes = new BYTE[iWidth * iHeight * bpp / 8];
	memcpy(*bmpBytes, bDataPointer, iWidth * iHeight * bpp / 8);

	FreeImage_Unload(dib);
	return true;
}

void CCubemap::Create(string sPositiveX, string sNegativeX, string sPositiveY, string sNegativeY, string sPositiveZ, string sNegativeZ)
{
	int iWidth, iHeight;

	glGenTextures(1, &m_uiTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_uiTexture);

	BYTE* pbImagePosX = nullptr, * pbImageNegX = nullptr, * pbImagePosY = nullptr, * pbImageNegY = nullptr, * pbImagePosZ = nullptr, * pbImageNegZ = nullptr;

	// Only uploads and frees a face if it actually loaded, so a missing file doesn't touch a null pointer
	if (LoadTexture(sPositiveX, &pbImagePosX, iWidth, iHeight)) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImagePosX);
		delete[] pbImagePosX;
	}
	if (LoadTexture(sNegativeX, &pbImageNegX, iWidth, iHeight)) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImageNegX);
		delete[] pbImageNegX;
	}
	if (LoadTexture(sPositiveY, &pbImagePosY, iWidth, iHeight)) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImagePosY);
		delete[] pbImagePosY;
	}
	if (LoadTexture(sNegativeY, &pbImageNegY, iWidth, iHeight)) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImageNegY);
		delete[] pbImageNegY;
	}
	if (LoadTexture(sPositiveZ, &pbImagePosZ, iWidth, iHeight)) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImagePosZ);
		delete[] pbImagePosZ;
	}
	if (LoadTexture(sNegativeZ, &pbImageNegZ, iWidth, iHeight)) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pbImageNegZ);
		delete[] pbImageNegZ;
	}

	glGenSamplers(1, &m_uiSampler);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glSamplerParameteri(m_uiSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_uiSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void CCubemap::Bind(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_uiTexture);
	glBindSampler(iTextureUnit, m_uiSampler);
}
// Release resources
void CCubemap::Release()
{
	glDeleteSamplers(1, &m_uiSampler);
	glDeleteTextures(1, &m_uiTexture);
}